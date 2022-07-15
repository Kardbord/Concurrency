#ifndef BENCHMARK
#define BENCHMARK

#include <ShardedUnorderedConcurrentMap.h>
#include <atomic>
#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

constexpr uint64_t default_benchmark_iterations = 1'000'000;

template <typename>
struct is_sharded : std::false_type {};

template <typename Key, typename Val>
struct is_sharded<::Concurrent::ShardedUnorderedMap<Key, Val>> : std::true_type {};

template <typename Key, typename Val, uint32_t ShardCount>
struct is_sharded<::Concurrent::ShardedUnorderedMap<Key, Val, ShardCount>> : std::true_type {};

template <typename T>
struct TypeParseTraits;

#define REGISTER_PARSE_TYPE(X) \
  template <>                  \
  struct TypeParseTraits<X> {  \
    static const char *name;   \
  };                           \
  const char *TypeParseTraits<X>::name = #X

// Register a map benchmark.
//   bname - the name to give this benchmark function
//   bfunc - the function which will be timed when the benchmark is invoked --
//           a lambda of the form [&test_map]() { /* do stuff with test_map */ }
// Invoke a registered benchmark using the INVOKE_BENCHMARK macro.
#define REGISTER_BENCHMARK(bname, bfunc)                                                                            \
  template <typename map_type>                                                                                      \
  ::Benchmark::Result bench_##bname(map_type &test_map, uint64_t const iterations = default_benchmark_iterations) { \
    ::Benchmark::Result r;                                                                                          \
    r.operation = #bname;                                                                                           \
    if constexpr (is_sharded<map_type>::value) {                                                                    \
      r.map_type    = "Sharded";                                                                                    \
      r.shard_count = std::to_string(test_map.shard_count());                                                       \
    } else {                                                                                                        \
      r.map_type    = "Unsharded";                                                                                  \
      r.shard_count = "N/A";                                                                                        \
    }                                                                                                               \
    r.key_type              = TypeParseTraits<typename map_type::key_type>::name;                                   \
    r.val_type              = TypeParseTraits<typename map_type::mapped_type>::name;                                \
    r.total_operations      = iterations;                                                                           \
    r.total_elapsed_ms      = ::Benchmark::bench(bfunc, iterations);                                                \
    r.avg_operations_per_ms = iterations / static_cast<double>(r.total_elapsed_ms.count());                         \
    return r;                                                                                                       \
  }

// Invoke a registered benchmark.
//   bname      - the name of a registered benchmark
//   test_map   - the map to run the registered benchmark function on.
//   return     - a populated ::Benchmark::Result object.
#define INVOKE_BENCHMARK(bname, test_map) bench_##bname(test_map)

// Invoke a registered benchmark.
//   bname      - the name of a registered benchmark
//   test_map   - the map to run the registered benchmark function on.
//   iterations - the number of times to run the registered benchmark function.
//   return     - a populated ::Benchmark::Result object.
#define INVOKE_BENCHMARK_ITR(bname, test_map, iterations) bench_##bname(test_map, iterations)

namespace Benchmark {

  template <typename Functor>
  std::chrono::milliseconds bench(Functor &&f, uint64_t const iterations = default_benchmark_iterations) {
    using ::std::chrono::duration_cast;
    using ::std::chrono::steady_clock;

    std::atomic_uint64_t itr = 0;

    std::vector<std::thread> threads;
    auto thread_func = [&itr, &iterations](Functor &&f) -> void {
      for ((void) itr; itr < iterations; ++itr) {
        f();
      }
    };

    auto start = steady_clock::now();
    for (uint32_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
      threads.emplace_back(thread_func, f);
    }

    for (auto &t: threads) {
      t.join();
    }
    return duration_cast<::std::chrono::milliseconds>(steady_clock::now() - start);
  }

  struct Result {
    ::std::string operation{};
    ::std::string map_type{};
    ::std::string key_type{};
    ::std::string val_type{};
    ::std::string shard_count{};
    uint64_t total_operations{};
    double avg_operations_per_ms{};
    ::std::chrono::milliseconds total_elapsed_ms{};
    uint32_t thread_count{std::thread::hardware_concurrency()};

    static std::string csv_header();
    std::string csv_row() const;

    static std::string results_to_csv(std::vector<Result> const &results);
  };

} // namespace Benchmark

#endif // BENCHMARK