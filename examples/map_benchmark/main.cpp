#include <Benchmark.h>
#include <ShardedUnorderedConcurrentMap.h>
#include <UnorderedConcurrentMap.h>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <vector>

using ::Concurrent::ShardedUnorderedMap;
using ::Concurrent::UnorderedMap;

REGISTER_PARSE_TYPE(int);

REGISTER_BENCHMARK(constructor, [&test_map]() { test_map = typename std::remove_reference<decltype(test_map)>::type(); })
REGISTER_BENCHMARK(empty, [&test_map]() { test_map.empty(); })
REGISTER_BENCHMARK(size, [&test_map]() { test_map.size(); })
REGISTER_BENCHMARK(clear, [&test_map]() { test_map.clear(); })
REGISTER_BENCHMARK(insert, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  using val_type = typename std::remove_reference<decltype(test_map)>::type::mapped_type;
  test_map.insert({key_type(), val_type()});
})

// TODO: benchmark additional types and methods.
int main() {
  UnorderedMap<int, int> m1;
  ShardedUnorderedMap<int, int> m2;
  std::vector<::Benchmark::Result> results;

  results.push_back(INVOKE_BENCHMARK(constructor, m1));
  results.push_back(INVOKE_BENCHMARK(constructor, m2));
  results.push_back(INVOKE_BENCHMARK(clear, m1));
  results.push_back(INVOKE_BENCHMARK(clear, m2));
  results.push_back(INVOKE_BENCHMARK(empty, m1));
  results.push_back(INVOKE_BENCHMARK(empty, m2));
  results.push_back(INVOKE_BENCHMARK(size, m1));
  results.push_back(INVOKE_BENCHMARK(size, m2));
  results.push_back(INVOKE_BENCHMARK(insert, m1));
  results.push_back(INVOKE_BENCHMARK(insert, m2));

  std::cout << ::Benchmark::Result::results_to_csv(results);
  return EXIT_SUCCESS;
}