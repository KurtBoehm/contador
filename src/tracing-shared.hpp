// This file is part of https://github.com/KurtBoehm/contador.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.

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
#include <utility>

#include <dlfcn.h>

#include <fmt/base.h>
#include <fmt/std.h>

using Free = void (*)(void*);
Free free_sys = nullptr;

inline std::atomic_size_t& get_max_rss_kb() {
  static std::atomic_size_t max_rss_kb{};
  return max_rss_kb;
}

inline std::pair<std::array<char, 8192>, std::size_t> read_proc(const char* p) {
  std::pair<std::array<char, 8192>, std::size_t> out{};
  FILE* file{};
  while ((file = std::fopen(p, "rb")) == nullptr) {
  }
  out.second = std::fread(out.first.data(), 1, out.first.size(), file);
  (void)std::fclose(file);
  return out;
}
inline std::size_t read_rss() {
  const auto [msg_arr, msg_len] = read_proc("/proc/self/smaps_rollup");
  const std::string_view msg{msg_arr.data(), msg_len};
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

inline std::size_t updated_max_rss() {
  const std::size_t rss = read_rss();
  auto& atom = get_max_rss_kb();
  std::size_t current = atom.load();
  while (!atom.compare_exchange_weak(current, std::max(current, rss))) {
  }
  return current;
}

inline std::mutex& free_init_mutex() {
  static std::mutex mutex{};
  return mutex;
}
inline Free get_free() {
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

#endif // SRC_TRACING_SHARED_HPP
