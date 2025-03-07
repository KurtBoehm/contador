#ifndef INCLUDE_CONTADOR_HPP
#define INCLUDE_CONTADOR_HPP

#include <cstddef>
#include <optional>

namespace contador {
// Maximum RSS in kibibytes
std::optional<std::size_t> max_rss();
} // namespace contador

#endif // INCLUDE_CONTADOR_HPP
