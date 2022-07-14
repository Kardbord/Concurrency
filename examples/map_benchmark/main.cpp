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
REGISTER_BENCHMARK(empty_when_empty, [&test_map]() { test_map.empty(); })
REGISTER_BENCHMARK(empty_when_not_empty, [&test_map]() { test_map.empty(); })
REGISTER_BENCHMARK(size, [&test_map]() { test_map.size(); })
REGISTER_BENCHMARK(clear, [&test_map]() { test_map.clear(); })
REGISTER_BENCHMARK(insert, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  using val_type = typename std::remove_reference<decltype(test_map)>::type::mapped_type;
  test_map.insert({key_type(), val_type()});
})
REGISTER_BENCHMARK(insert_or_assign_existing, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  using val_type = typename std::remove_reference<decltype(test_map)>::type::mapped_type;
  test_map.insert_or_assign(key_type(), val_type());
})
REGISTER_BENCHMARK(insert_or_assign_not_existing, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  using val_type = typename std::remove_reference<decltype(test_map)>::type::mapped_type;
  test_map.insert_or_assign(key_type(), val_type());
})
REGISTER_BENCHMARK(erase, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  test_map.erase(key_type());
})
REGISTER_BENCHMARK(swap_with_empty, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  test_map.swap(tmp);
})
REGISTER_BENCHMARK(merge_with_empty, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  test_map.merge(tmp);
})
REGISTER_BENCHMARK(merge_with_empty_internal_map_type, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type::internal_map_type tmp;
  test_map.merge(tmp);
})
// TODO: benchmark merge_with_multimap
REGISTER_BENCHMARK(at, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  test_map.at(key_type());
})
REGISTER_BENCHMARK(subscript_operator, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  test_map[key_type()];
})
REGISTER_BENCHMARK(count, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  (void) test_map.count(key_type());
})
REGISTER_BENCHMARK(find, [&test_map]() {
  using key_type = typename std::remove_reference<decltype(test_map)>::type::key_type;
  (void) test_map.find(key_type());
})
REGISTER_BENCHMARK(data, [&test_map]() { (void) test_map.data(); })
REGISTER_BENCHMARK(load_factor, [&test_map]() { (void) test_map.load_factor(); })
REGISTER_BENCHMARK(get_max_load_factor, [&test_map]() { (void) test_map.max_load_factor(); })
REGISTER_BENCHMARK(set_max_load_factor, [&test_map]() { test_map.max_load_factor(0.5); })
REGISTER_BENCHMARK(rehash, [&test_map]() { test_map.rehash(20); })
REGISTER_BENCHMARK(reserve, [&test_map]() { test_map.reserve(20); })

int main() {
  UnorderedMap<int, int> m1;
  ShardedUnorderedMap<int, int> m2;
  std::vector<::Benchmark::Result> results;

  results.push_back(INVOKE_BENCHMARK(constructor, m1));
  results.push_back(INVOKE_BENCHMARK(constructor, m2));
  results.push_back(INVOKE_BENCHMARK(empty_when_empty, m1));
  results.push_back(INVOKE_BENCHMARK(empty_when_empty, m2));
  results.push_back(INVOKE_BENCHMARK(clear, m1));
  results.push_back(INVOKE_BENCHMARK(clear, m2));
  results.push_back(INVOKE_BENCHMARK(size, m1));
  results.push_back(INVOKE_BENCHMARK(size, m2));
  results.push_back(INVOKE_BENCHMARK(insert, m1));
  results.push_back(INVOKE_BENCHMARK(insert, m2));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_existing, m1));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_existing, m2));
  results.push_back(INVOKE_BENCHMARK(empty_when_not_empty, m1));
  results.push_back(INVOKE_BENCHMARK(empty_when_not_empty, m2));
  results.push_back(INVOKE_BENCHMARK(erase, m1));
  results.push_back(INVOKE_BENCHMARK(erase, m2));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_not_existing, m1));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_not_existing, m2));
  results.push_back(INVOKE_BENCHMARK(swap_with_empty, m1));
  results.push_back(INVOKE_BENCHMARK(swap_with_empty, m2));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty, m1));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty, m2));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty_internal_map_type, m1));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty_internal_map_type, m2));
  results.push_back(INVOKE_BENCHMARK(subscript_operator, m1));
  results.push_back(INVOKE_BENCHMARK(subscript_operator, m2));
  results.push_back(INVOKE_BENCHMARK(at, m1));
  results.push_back(INVOKE_BENCHMARK(at, m2));
  results.push_back(INVOKE_BENCHMARK(count, m1));
  results.push_back(INVOKE_BENCHMARK(count, m2));
  results.push_back(INVOKE_BENCHMARK(find, m1));
  results.push_back(INVOKE_BENCHMARK(find, m2));
  results.push_back(INVOKE_BENCHMARK(data, m1));
  results.push_back(INVOKE_BENCHMARK(data, m2));
  results.push_back(INVOKE_BENCHMARK(load_factor, m1));
  results.push_back(INVOKE_BENCHMARK(load_factor, m2));
  results.push_back(INVOKE_BENCHMARK(get_max_load_factor, m1));
  results.push_back(INVOKE_BENCHMARK(get_max_load_factor, m2));
  results.push_back(INVOKE_BENCHMARK(set_max_load_factor, m1));
  results.push_back(INVOKE_BENCHMARK(set_max_load_factor, m2));
  results.push_back(INVOKE_BENCHMARK(rehash, m1));
  results.push_back(INVOKE_BENCHMARK(rehash, m2));
  results.push_back(INVOKE_BENCHMARK(reserve, m1));
  results.push_back(INVOKE_BENCHMARK(reserve, m2));

  std::cout << ::Benchmark::Result::results_to_csv(results);
  return EXIT_SUCCESS;
}