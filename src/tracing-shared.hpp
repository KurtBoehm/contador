#ifndef SRC_TRACING_SHARED_HPP
#define SRC_TRACING_SHARED_HPP

#include <algorithm>
#include <array>
#include <atomic>
#include <charconv>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <mutex>
#include <ranges>
#include <string_view>

#include <dlfcn.h>

#include <fmt/base.h>
#include <fmt/std.h>

namespace {
using Free = void (*)(void*);
Free free_sys = nullptr;

std::atomic_size_t& get_max_rss_kb() {
  static std::atomic_size_t max_rss_kb{};
  return max_rss_kb;
}

std::string_view read_proc(const char* p) {
  thread_local std::array<char, 8192> buf{};
  FILE* file{};
  while (!(file = std::fopen(p, "rb"))) {
  }
  const auto num = std::fread(buf.data(), 1, buf.size(), file);
  std::fclose(file);
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

std::size_t updated_max_rss() {
  const std::size_t rss = read_rss();
  auto& atom = get_max_rss_kb();
  std::size_t current = atom.load();
  while (!atom.compare_exchange_weak(current, std::max(current, rss))) {
  }
  return current;
}

std::mutex& free_init_mutex() {
  static std::mutex mutex{};
  return mutex;
}
Free get_free() {
  if (free_sys == nullptr) {
    std::lock_guard lock{free_init_mutex()};
    if (free_sys == nullptr) {
      free_sys = reinterpret_cast<Free>(dlsym(RTLD_NEXT, "free"));
      if (free_sys == nullptr) {
        fmt::print(stderr, "Error in dlsym(free): {}\n", dlerror());
      }
    }
  }
  return free_sys;
}
} // namespace

#endif // SRC_TRACING_SHARED_HPP
