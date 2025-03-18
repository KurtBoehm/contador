#ifndef INCLUDE_CONTADOR_HPP
#define INCLUDE_CONTADOR_HPP

#include <cstddef>
#include <optional>

namespace contador {
struct Tracer {
  Tracer();
  Tracer(const Tracer&) = delete;
  Tracer(Tracer&&) = delete;
  Tracer& operator=(const Tracer&) = delete;
  Tracer& operator=(Tracer&&) = delete;
  ~Tracer();

  // Maximum RSS in kibibytes
  std::optional<std::size_t> max_rss();
};
} // namespace contador

#endif // INCLUDE_CONTADOR_HPP
