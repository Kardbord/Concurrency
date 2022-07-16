#include <Benchmark.h>
#include <ShardedUnorderedConcurrentMap.h>
#include <UnorderedConcurrentMap.h>
#include <cstdlib>
#include <iostream>
#include <type_traits>
#include <vector>

using ::Concurrent::ShardedUnorderedMap;
using ::Concurrent::UnorderedMap;

template <typename map_type>
void teardown_test_map(map_type &m) {
  m.clear();
}

template <typename map_type>
void void_func(map_type &m) {
  (void) m;
}

constexpr uint64_t setup_test_map_size = 1'000;

template <typename map_type>
auto get_map_init_values() -> const std::vector<typename map_type::value_type> & {
  using key_type = typename map_type::key_type;
  using val_type = typename map_type::mapped_type;
  static_assert(std::is_same_v<key_type, int> || std::is_same_v<val_type, int>, "Unknown key/val combination.");

  static thread_local std::atomic_bool initialized = false;
  static thread_local std::vector<typename map_type::value_type> v;
  if (initialized) {
    return v;
  }
  initialized = true;
  v.clear();
  v.push_back({key_type(), val_type()});
  for (uint64_t i = 0; i < setup_test_map_size; ++i) {
    v.push_back({i, i});
  }
  return v;
}

template <typename map_type>
void setup_test_map(map_type &m) {
  using key_type = typename map_type::key_type;
  using val_type = typename map_type::mapped_type;
  static_assert(std::is_same_v<key_type, int> || std::is_same_v<val_type, int>, "Unknown key/val combination.");
  for (auto const &mapped: get_map_init_values<map_type>()) {
    m.insert({mapped.first, mapped.second});
  }
}

REGISTER_PARSE_TYPE(int);

REGISTER_BENCHMARK(default_constructor, 1, [&test_map]() { test_map = typename std::remove_reference<decltype(test_map)>::type(); })
REGISTER_BENCHMARK(empty_when_empty, 1, [&test_map]() { test_map.empty(); })
REGISTER_BENCHMARK(empty_when_not_empty, 1, [&test_map]() { test_map.empty(); })
REGISTER_BENCHMARK(size_when_empty, 1, [&test_map]() { test_map.size(); })
REGISTER_BENCHMARK(size, 1, [&test_map]() { test_map.size(); })
REGISTER_BENCHMARK(clear_when_empty, 1, [&test_map]() { test_map.clear(); })
REGISTER_BENCHMARK(clear, 1, [&test_map]() { test_map.clear(); })
REGISTER_BENCHMARK(insert_when_empty, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    test_map.insert({key, val});
  }
})
REGISTER_BENCHMARK(insert_when_key_exists, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    test_map.insert({key, val});
  }
})
REGISTER_BENCHMARK(insert_or_assign_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    test_map.insert_or_assign(key, val);
  }
})
REGISTER_BENCHMARK(insert_or_assign_not_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    test_map.insert_or_assign(key, val);
  }
})
REGISTER_BENCHMARK(erase_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map.erase(key);
  }
})
REGISTER_BENCHMARK(erase_not_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map.erase(key);
  }
})
REGISTER_BENCHMARK(swap_with_empty, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  test_map.swap(tmp);
})
REGISTER_BENCHMARK(merge_with_empty, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  test_map.merge(tmp);
})
REGISTER_BENCHMARK(merge_with_empty_internal_map_type, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type::internal_map_type tmp;
  test_map.merge(tmp);
})
REGISTER_BENCHMARK(swap, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  setup_test_map(tmp);
  test_map.swap(tmp);
})
REGISTER_BENCHMARK(merge, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type tmp;
  setup_test_map(tmp);
  test_map.merge(tmp);
})
REGISTER_BENCHMARK(merge_with_internal_map_type, 1, [&test_map]() {
  typename std::remove_reference<decltype(test_map)>::type::internal_map_type tmp;
  setup_test_map(tmp);
  test_map.merge(tmp);
})
// TODO: benchmark merge_with_multimap
REGISTER_BENCHMARK(at, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map.at(key);
  }
})
REGISTER_BENCHMARK(subscript_operator_not_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map[key];
  }
})
REGISTER_BENCHMARK(subscript_operator_existing, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map[key];
  }
})
REGISTER_BENCHMARK(count, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map.count(key);
  }
})
REGISTER_BENCHMARK(find, setup_test_map_size, [&test_map]() {
  for (auto const &[key, val]: get_map_init_values<map_type>()) {
    (void) val;
    test_map.find(key);
  }
})
REGISTER_BENCHMARK(data, 1, [&test_map]() { (void) test_map.data(); })
REGISTER_BENCHMARK(load_factor, 1, [&test_map]() { (void) test_map.load_factor(); })
REGISTER_BENCHMARK(get_max_load_factor, 1, [&test_map]() { (void) test_map.max_load_factor(); })
REGISTER_BENCHMARK(set_max_load_factor, 1, [&test_map]() { test_map.max_load_factor(0.5); })
REGISTER_BENCHMARK(rehash, 1, [&test_map]() { test_map.rehash(setup_test_map_size * 2); })
REGISTER_BENCHMARK(reserve, 1, [&test_map]() { test_map.reserve(setup_test_map_size * 2); })

int main() {
  UnorderedMap<int, int> m1;
  ShardedUnorderedMap<int, int> m2;
  std::vector<::Benchmark::Result> results;

  results.push_back(INVOKE_BENCHMARK(default_constructor, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(default_constructor, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(empty_when_empty, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(empty_when_empty, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(empty_when_not_empty, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(empty_when_not_empty, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(size_when_empty, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(size_when_empty, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(size, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(size, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(clear_when_empty, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(clear_when_empty, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(clear, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(clear, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_when_empty, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_when_empty, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_when_key_exists, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_when_key_exists, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_existing, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_existing, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_not_existing, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(insert_or_assign_not_existing, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(erase_existing, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(erase_existing, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(erase_not_existing, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(erase_not_existing, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(swap_with_empty, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(swap_with_empty, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty_internal_map_type, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_empty_internal_map_type, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(swap, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(swap, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_internal_map_type, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(merge_with_internal_map_type, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(at, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(at, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(subscript_operator_not_existing, m1, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(subscript_operator_not_existing, m2, void_func, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(subscript_operator_existing, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(subscript_operator_existing, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(count, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(count, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(find, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(find, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(data, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(data, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(load_factor, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(load_factor, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(get_max_load_factor, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(get_max_load_factor, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(set_max_load_factor, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(set_max_load_factor, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(rehash, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(rehash, m2, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(reserve, m1, setup_test_map, teardown_test_map));
  results.push_back(INVOKE_BENCHMARK(reserve, m2, setup_test_map, teardown_test_map));

  std::cout << ::Benchmark::Result::results_to_csv(results);
  return EXIT_SUCCESS;
}