#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <print>
#include <random>
#include <thread>
#include <vector>

#include "contador/contador.hpp"

int main() {
  constexpr std::size_t tnum = 4;
  constexpr std::size_t reps = 16;
  constexpr std::size_t maxms = 200;
  constexpr std::size_t maxbytes = 2'000'000;

  contador::Tracer tracer{};

  {
    std::vector<std::jthread> threads{};
    threads.reserve(tnum);
    for (std::size_t i = 0; i < tnum; ++i) {
      threads.emplace_back([] {
        using Dur = std::chrono::milliseconds;
        std::mt19937_64 rng{std::random_device{}()};
        std::uniform_int_distribution<Dur::rep> sleep_dist{0, maxms};
        std::uniform_int_distribution<std::size_t> bytes_dist{0, maxbytes};

        void* data;
        {
          const std::size_t bnum = bytes_dist(rng);
          data = std::malloc(bnum);
        }
        for (std::size_t j = 1; j < reps; ++j) {
          std::this_thread::sleep_for(Dur{sleep_dist(rng)});
          std::free(data);

          const std::size_t bnum = bytes_dist(rng);
          data = std::malloc(bnum);
        }
        std::free(data);
      });
    }
  }

  if (auto bytes = tracer.max_rss(); bytes.has_value()) {
    std::print("max_rss: {}\n", *bytes);
  } else {
    std::print("no max_rss\n");
  }
}
