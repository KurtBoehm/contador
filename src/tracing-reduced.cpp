// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <cstddef>
#include <cstdlib>
#include <optional>
#include <thread>
#include <utility>

#include "contador/contador.hpp"

#include "tracing-shared.hpp"

namespace {
using MainData = std::pair<std::thread::id, std::size_t>;

std::optional<MainData>& get_main_data() {
  static std::optional<MainData> data{};
  return data;
}
} // namespace

extern "C" {
void free(void* p) {
  const auto tid = std::this_thread::get_id();
  if (auto& data_opt = get_main_data(); data_opt.has_value() && data_opt->first == tid) {
    auto& depth = data_opt->second;
    ++depth;
    if (depth == 1) {
      // Outermost free call on the main thread.
      updated_max_rss();
    }
    if (depth > 2) {
      // More than 2 nested frees on the main thread is considered an error.
      std::abort();
    }
    --depth;
  }

  auto free_ptr = get_free();
  free_ptr(p);
}
} // extern "C"

[[gnu::weak]] contador::Tracer::Tracer() {
  get_main_data().emplace(std::this_thread::get_id(), 0);
}
[[gnu::weak]] contador::Tracer::~Tracer() {
  get_main_data().reset();
}

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return updated_max_rss();
}
