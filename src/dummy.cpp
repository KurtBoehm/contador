#include <cstddef>
#include <optional>

#include "contador/contador.hpp"

[[gnu::weak]] std::optional<std::size_t> contador::max_rss() {
  return std::nullopt;
}
