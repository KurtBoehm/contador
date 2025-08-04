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
#include <utility>

#include <unistd.h>

#include "contador/contador.hpp"

#include "tracing-shared.hpp"

namespace {
std::mutex& counters_mutex() {
  static std::mutex mutex{};
  return mutex;
}
std::optional<std::deque<std::pair<int, std::size_t>>>& get_counters() {
  static std::optional<std::deque<std::pair<int, std::size_t>>> data{};
  return data;
}
} // namespace

extern "C" {
void free(void* p) {
  const auto tid = gettid();
  auto& counters_opt = get_counters();

  if (counters_opt.has_value()) {
    auto& counters = *counters_opt;

    auto it = std::ranges::find_if(counters, [tid](auto p) { return p.first == tid; });
    std::size_t* iptr{};
    if (it == counters.end()) {
      std::lock_guard lock{counters_mutex()};
      iptr = &counters.emplace_back(tid, 0).second;
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
