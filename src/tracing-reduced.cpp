// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <cstddef>
#include <cstdlib>
#include <optional>
#include <utility>

#include <unistd.h>

#include "contador/contador.hpp"

#include "tracing-shared.hpp"

namespace {
std::optional<std::pair<int, std::size_t>>& get_main_data() {
  static std::optional<std::pair<int, std::size_t>> data{};
  return data;
}
} // namespace

extern "C" {
void free(void* p) {
  if (auto& data_opt = get_main_data(); data_opt.has_value() && data_opt->first == gettid()) {
    auto& i = data_opt->second;
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
  get_main_data().emplace(gettid(), 0);
}
[[gnu::weak]] contador::Tracer::~Tracer() {
  get_main_data().reset();
}

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return updated_max_rss();
}
