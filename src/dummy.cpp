#include <cstddef>
#include <optional>

#include "contador/contador.hpp"

[[gnu::weak]] contador::Tracer::Tracer() {}
[[gnu::weak]] contador::Tracer::~Tracer() {}

[[gnu::weak]] std::optional<std::size_t> contador::Tracer::max_rss() {
  return std::nullopt;
}
