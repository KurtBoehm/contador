#define _GNU_SOURCE

#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <ranges>
#include <string_view>

#include <dlfcn.h>

#include <fmt/base.h>
#include <fmt/ranges.h>
#include <fmt/std.h>

#define MYLLOC_MALLOC false

static bool rec = false;
static std::array<char, 8192> buf{};

std::string_view read_proc(const std::filesystem::path& p) {
  FILE* file = std::fopen(p.c_str(), "rb");
  const auto num = std::fread(buf.data(), 1, buf.size(), file);
  return std::string_view{buf.data(), num};
}

using Malloc = void* (*)(size_t);
using Free = void (*)(void*);

#if MYLLOC_MALLOC
static Malloc malloc_sys = nullptr;
#endif
static Free free_sys = nullptr;

template<typename TFun>
static void fun_init(TFun* target, const char* name) {
  *target = reinterpret_cast<TFun>(dlsym(RTLD_NEXT, name));
  if (*target == nullptr) {
    fprintf(stderr, "Error in dlsym(%s): %s\n", name, dlerror());
  }
}

extern "C" {
#if MYLLOC_MALLOC
void* malloc(size_t size) {
  if (malloc_sys == nullptr) {
    fun_init(&malloc_sys, "malloc");
  }

  fprintf(stderr, "malloc(%zu) = ", size);
  void* p = malloc_sys(size);
  fprintf(stderr, "%p\n", p);
  return p;
}
#endif

static std::size_t max_rss_kb{};

void free(void* p) {
  if (free_sys == nullptr) {
    fun_init(&free_sys, "free");
  }
#if MYLLOC_MALLOC
  fprintf(stderr, "free(%p)\n", p);
#endif
  if (!rec) {
    rec = true;
    const auto msg = read_proc("/proc/self/smaps_rollup");
    // fmt::print(stderr, "{}\n", msg);
    auto lines = std::views::split(msg, '\n');
    auto line = [&] {
      auto it = lines.begin();
      std::advance(it, 1);
      auto range = *it;
      return std::string_view{range.data(), range.size()};
    }();
    auto parts = line | std::views::split(':');
    auto it = parts.begin();
    auto rkey = *it++;
    auto rval = *it++;
    auto key = std::string_view{rkey.data(), rkey.size()};
    auto val = std::string_view{rval.data(), rval.size()};
    std::size_t ival{};
    val.remove_prefix(val.find_first_not_of(' '));
    // TODO Error checking!
    std::from_chars(val.begin(), val.end(), ival);
    max_rss_kb = std::max(max_rss_kb, ival);
    fmt::print(stderr, "maxrss: {} KiB\n", max_rss_kb);
    rec = false;
  }
  free_sys(p);
}
}
