# Contador 🧮: Precise Maximum RSS Tracing

Contador (Spanish “counter”) provides a reasonably convenient mechanism to measure the memory consumption (_maximum resident set size_) of a given program with high accuracy.
It relies on the information provided in `/proc/self/smaps_rollup` (specifically the value associated with the `Rss` key) and is therefore Linux-specific.
Results so determined are much more accurate than `getrusage().ru_maxrss`, which can result bogus results for multithreaded programs with a short runtime.
However, repeatedly querying `smaps_rollup` is much more expensive than calling `getrusage` and should therefore not be performed during regular program execution.

## Usage

To use Contador, the following steps are required:

1. Include `contador.hpp` in your program, which defines a class named `contador::Tracer`.
2. In your main thread (ideally at the start of the `main` function), create an instance of `contador::Tracer`.
3. To query the maximum RSS up to a given point in your program, call `contador::Tracer::max_rss` on the main thread, which either returns the maximum RSS or `nullopt` if it is in its _dummy_ mode (see next section).
4. Link with one of the libraries that represents one of the tracing modes described in the next section. The default approach is to link with `contador_dummy` and to preload one of the other libraries to measure memory consumption.

The following code snippet can be used as a template:

```cpp
#include <print> // std::print (C++23)
#include <contador/contador.hpp>

int main() {
  contador::Tracer contador{};
  // do work…
  if (const auto max_rss = contador.max_rss(); max_rss.has_value()) {
    std::print("maximum RSS: {}\n", *max_rss);
  }
}
```

When one of the tracing modes is used (see next section), this will print the maximum RSS, and nothing is printed otherwise.

## Tracing Modes

Contador contains three tracing modes that target a different frequency of queries to the current RSS, which are listed together with the names of the corresponding libraries built by Contador:

- _Dummy_ (`contador_dummy`): Does not measure memory consumption and therefore does not impact performance. `contador::Tracer::max_rss` always returns `std::nullopt`.
- _Tracing_ (`contador`): The maximum RSS is updated every time `free` is called. This gives the most precise results but can be prohibitively expensive in multithreaded programs.
- _Reduced Tracing_ (`contador_reduced`): The maximum RSS is updated every time `free` is called _on the main thread_ (or, more precisely, the thread on which `contador::Tracer` was created). This is much less expensive than the _Tracing_ mode in multithreaded programs and does not miss allocations in most cases. However, thread-local allocations might be missed.

The suggested way of using Contador is to link your program with `contador_dummy.so` to be able to use the program without performance impact.
To measure the memory consumption, one then runs the program with the environment variable `LD_PRELOAD` set to the path of `contador.so` of `contador-reduced.so`, which dynamically links the program to the corresponding library and uses the corresponding tracing mode for this execution of the program.
