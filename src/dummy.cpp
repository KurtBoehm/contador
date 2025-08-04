// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

#include <cstddef>
#include <optional>

#include "contador/contador.hpp"

[[gnu::weak]] contador::Tracer::Tracer() = default;
[[gnu::weak]] contador::Tracer::~Tracer() = default;

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return std::nullopt;
}
