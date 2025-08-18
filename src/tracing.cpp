// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <thread>
#include <utility>
#include <vector>

#include "contador/contador.hpp"

#include "tracing-shared.hpp"

namespace {
using Counter = std::pair<std::thread::id, std::size_t>;

std::shared_mutex& counters_mutex() {
  static std::shared_mutex mutex{};
  return mutex;
}
std::optional<std::vector<Counter>>& get_counters() {
  static std::optional<std::vector<Counter>> data{};
  return data;
}
} // namespace

extern "C" {
void free(void* p) {
  const auto tid = std::this_thread::get_id();

  std::shared_lock shlck{counters_mutex()};
  auto& counters_opt = get_counters();
  if (counters_opt.has_value()) {
    auto& counters = *counters_opt;
    auto it = std::ranges::find_if(counters, [tid](Counter p) { return p.first == tid; });

    std::size_t* iptr{};
    if (it == counters.end()) {
      shlck.unlock();
      {
        std::unique_lock lock{counters_mutex()};
        iptr = &counters.emplace_back(tid, 0).second;
      }
      shlck.lock();
    } else {
      iptr = &it->second;
    }
    auto& i = *iptr;

    ++i;
    if (i == 1) {
      updated_max_rss();
    }
    if (i > 2) {
      std::abort();
    }
    --i;
  }
  shlck.unlock();

  auto free_ptr = get_free();
  free_ptr(p);
}
}

[[gnu::weak]] contador::Tracer::Tracer() {
  get_counters().emplace();
}
[[gnu::weak]] contador::Tracer::~Tracer() {
  get_counters().reset();
}

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return updated_max_rss();
}
