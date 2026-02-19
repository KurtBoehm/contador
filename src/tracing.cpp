// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <deque>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <utility>

#include "contador/contador.hpp"

#include "tracing-shared.hpp"

namespace {
using Counter = std::pair<std::thread::id, std::size_t>;

std::shared_mutex& counters_mutex() {
  static std::shared_mutex mutex{};
  return mutex;
}
std::optional<std::deque<Counter>>& get_counters() {
  static std::optional<std::deque<Counter>> data{};
  return data;
}

// Get or create the per-thread counter entry under lock.
std::size_t& get_thread_counter(std::thread::id tid, std::deque<Counter>& counters) {
  {
    std::shared_lock lock{counters_mutex()};
    const auto it =
      std::ranges::find_if(counters, [tid](const Counter& p) { return p.first == tid; });
    if (it != counters.end()) {
      return it->second;
    }
  }

  {
    std::unique_lock lock{counters_mutex()};
    const auto it =
      std::ranges::find_if(counters, [tid](const Counter& p) { return p.first == tid; });
    if (it != counters.end()) {
      return it->second;
    }
    return counters.emplace_back(tid, 0).second;
  }
}
} // namespace

extern "C" {
void free(void* p) {
  const auto tid = std::this_thread::get_id();

  auto& counters_opt = get_counters();

  if (counters_opt.has_value()) {
    std::size_t& counter = get_thread_counter(tid, *counters_opt);

    ++counter;
    if (counter == 1) {
      // First level: this is the "outermost" free call on this thread.
      updated_max_rss();
    }
    if (counter > 2) {
      // More than 2 nested frees on the same thread is considered an error.
      std::abort();
    }
    --counter;
  }

  auto free_ptr = get_free();
  free_ptr(p);
}
} // extern "C"

[[gnu::weak]] contador::Tracer::Tracer() {
  get_counters().emplace();
}
[[gnu::weak]] contador::Tracer::~Tracer() {
  get_counters().reset();
}

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return updated_max_rss();
}
