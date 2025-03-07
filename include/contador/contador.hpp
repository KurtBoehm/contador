#ifndef INCLUDE_CONTADOR_HPP
#define INCLUDE_CONTADOR_HPP

#ifndef CONTADOR_TRACING
#error "Define CONTADOR_TRACING!"
#endif
#ifndef CONTADOR_VERBOSE
#define CONTADOR_VERBOSE false
#endif

#include <cstddef>

namespace contador {
// Maximum RSS in kibibytes
std::size_t max_rss();
} // namespace contador

#endif // INCLUDE_CONTADOR_HPP
