#include <algorithm>
#include <array>
#include <charconv>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <optional>
#include <ranges>
#include <string_view>

#include <dlfcn.h>

#include <fmt/base.h>
#include <fmt/std.h>

#include "contador/contador.hpp"

namespace {
std::size_t max_rss_kb{};
bool rec = false;
std::array<char, 8192> buf{};

std::string_view read_proc(const std::filesystem::path& p) {
  FILE* file = std::fopen(p.c_str(), "rb");
  const auto num = std::fread(buf.data(), 1, buf.size(), file);
  return std::string_view{buf.data(), num};
}
std::size_t read_rss() {
  const auto msg = read_proc("/proc/self/smaps_rollup");
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
  if (key != "Rss") {
    fmt::print(stderr, "Key: {}\n", key);
    std::abort();
  }
  auto val = std::string_view{rval.data(), rval.size()};
  std::size_t ival{};
  val.remove_prefix(val.find_first_not_of(' '));
  // TODO Error checking!
  std::from_chars(val.begin(), val.end(), ival);
  return ival;
}

using Free = void (*)(void*);
Free free_sys = nullptr;

template<typename TFun>
void fun_init(TFun* target, const char* name) {
  *target = reinterpret_cast<TFun>(dlsym(RTLD_NEXT, name));
  if (*target == nullptr) {
    fprintf(stderr, "Error in dlsym(%s): %s\n", name, dlerror());
  }
}
} // namespace

extern "C" {
void free(void* p) {
  if (free_sys == nullptr) {
    fun_init(&free_sys, "free");
  }
  if (!rec) {
    rec = true;
    max_rss_kb = std::max(max_rss_kb, read_rss());
    rec = false;
  }
  free_sys(p);
}
}

[[gnu::weak]] std::optional<std::size_t> contador::max_rss() {
  max_rss_kb = std::max(max_rss_kb, read_rss());
  return max_rss_kb;
}
