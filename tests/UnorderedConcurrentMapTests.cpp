#include <ShardedUnorderedConcurrentMap.h>
#include <UnorderedConcurrentMap.h>
#include <algorithm>
#include <gtest/gtest.h>
#include <string>
#include <type_traits>

namespace {
  using ::Concurrent::ShardedUnorderedMap;
  using ::Concurrent::UnorderedMap;

  // Custom struct for use as a map value.
  struct Foo {
    Foo() = default;
    Foo(int a, std::string b) : m_a(a), m_b(b) {}

    int m_a{};
    std::string m_b{};
  };
  bool operator==(const Foo &lhs, const Foo &rhs) { return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b; }
  // bool operator!=(const Foo &lhs, const Foo &rhs) { return !(lhs == rhs); }
  // bool operator==(const Foo &lhs, const Foo &&rhs) { return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b; }
  // bool operator!=(const Foo &lhs, const Foo &&rhs) { return !(lhs == rhs); }
  struct FooHash {
    size_t operator()(const Foo &foo) const { return std::hash<int>()(foo.m_a) ^ std::hash<std::string>()(foo.m_b); }
  };

  // helper constant for static_asserts in constexpr flow control.
  template <typename...>
  static inline constexpr bool always_false_v = false;

  template <typename map_type>
  static auto initialize_test_map() -> map_type {
    using key_type    = typename map_type::key_type;
    using mapped_type = typename map_type::mapped_type;
    if constexpr (std::is_same_v<key_type, std::string> && std::is_integral_v<mapped_type>) {
      return map_type{
          {"foo", 1},
          {"bar", 2},
          {"baz", 3},
      };
    } else if constexpr (std::is_same_v<key_type, std::string> && std::is_floating_point_v<mapped_type>) {
      return map_type{
          {"foo", 1.1},
          {"bar", 2.1},
          {"baz", 3.1},
      };
    } else if constexpr (std::is_same_v<key_type, std::string> && std::is_same_v<mapped_type, std::string>) {
      return map_type{
          {"foo", "qux"},
          {"bar", "quux"},
          {"baz", "quuux"},
      };
    } else if constexpr (std::is_integral_v<key_type> && std::is_integral_v<mapped_type>) {
      return map_type{
          {1, 1},
          {2, 2},
          {3, 3},
      };
    } else if constexpr (std::is_integral_v<key_type> && std::is_same_v<mapped_type, std::string>) {
      return map_type{
          {1, "foo"},
          {2, "bar"},
          {3, "baz"},
      };
    } else if constexpr (std::is_integral_v<key_type> && std::is_same_v<mapped_type, Foo>) {
      return map_type{
          {1, Foo(1, "a")},
          {2, Foo(2, "b")},
      };
    } else if constexpr (std::is_same_v<key_type, Foo> && std::is_integral_v<mapped_type>) {
      return map_type{
          {Foo(1, "a"), 1},
          {Foo(2, "b"), 2},
      };
    } else {
      static_assert(always_false_v<key_type, mapped_type>, "Unhandled <key_type, mapped_type> combination.");
    }
  }

  // Common test cases for both
  // ::Concurrent::ShardedUnorderedMap, and
  // ::Concurrent::UnorderedMap.
  template <typename T>
  class CommonConcurrentUnorderedMapTests : public ::testing::Test {};
  class UnshardedConcurrentUnorderedMapTests : public ::testing::Test {};
  class ShardedConcurrentUnorderedMapTests : public ::testing::Test {};

  TYPED_TEST_SUITE_P(CommonConcurrentUnorderedMapTests);
  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, DefaultConstructor) {
    using map_type = TypeParam;
    using key_type = typename map_type::key_type;
    map_type umap;
    constexpr typename map_type::size_type expected_size = 0;
    ASSERT_EQ(expected_size, umap.size());
    ASSERT_TRUE(umap.empty());

    try {
      umap.at(key_type());
      FAIL() << "Expected std::out_of_range.";
    } catch (std::out_of_range const &) {
    } catch (...) {
      FAIL() << "Expected std::out_of_range.";
    }

    try {
      umap.at(key_type());
      FAIL() << "Expected std::out_of_range.";
    } catch (std::out_of_range const &) {
    } catch (...) {
      FAIL() << "Expected std::out_of_range.";
    }

    ASSERT_EQ(0, umap.erase(key_type())) << "Expected no elements to be removed.";
    ASSERT_EQ(0, umap.count(key_type())) << "Expected no elements to be found.";
    ASSERT_FALSE(umap.find(key_type())) << "Expected no elements to be found.";

    auto data = umap.data();
    ASSERT_EQ(expected_size, data.size());
    ASSERT_TRUE(data.empty());

    ASSERT_EQ(map_type(), umap);
    ASSERT_FALSE(umap != map_type());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, CopyConstructor) {
    using map_type = TypeParam;

    auto umap1 = initialize_test_map<map_type>();
    ASSERT_FALSE(umap1.empty());
    auto umap2{umap1};
    ASSERT_EQ(umap1, umap2);
    auto umap3(umap2);
    ASSERT_EQ(umap1, umap3);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, MoveConstructor) {
    using map_type = TypeParam;

    auto umap1 = initialize_test_map<map_type>();
    ASSERT_FALSE(umap1.empty());
    auto umap2{initialize_test_map<map_type>()};
    ASSERT_EQ(umap1, umap2);
    auto umap3(initialize_test_map<map_type>());
    ASSERT_EQ(umap1, umap3);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, CopyAssignment) {
    using map_type = TypeParam;

    auto umap1 = initialize_test_map<map_type>();
    ASSERT_FALSE(umap1.empty());
    auto umap2 = umap1;
    ASSERT_EQ(umap1, umap2);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, MoveAssignment) {
    using map_type = TypeParam;

    auto umap = initialize_test_map<map_type>();
    ASSERT_FALSE(umap.empty());
    auto old_data = umap.data();
    umap          = initialize_test_map<map_type>();
    ASSERT_EQ(old_data, umap.data());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, get_allocator) {
    using map_type = TypeParam;

    auto umap     = initialize_test_map<map_type>();
    auto max_size = umap.get_allocator().max_size();
    ASSERT_NE(0, max_size);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, empty) {
    using map_type = TypeParam;

    map_type m;
    ASSERT_TRUE(m.empty());
    m = initialize_test_map<map_type>();
    ASSERT_FALSE(m.empty());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, size) {
    using map_type = TypeParam;

    map_type m;
    ASSERT_EQ(0, m.size());
    m = initialize_test_map<map_type>();
    ASSERT_LT(0, m.size());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, clear) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    ASSERT_FALSE(m.empty());
    m.clear();
    ASSERT_TRUE(m.empty());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, insert) {
    using map_type    = TypeParam;
    using value_type  = typename map_type::value_type;
    using key_type    = typename map_type::key_type;
    using mapped_type = typename map_type::mapped_type;
    using node_type   = typename map_type::node_type;

    // insert(const value_type &value)
    {
      map_type m;
      value_type v;
      ASSERT_TRUE(m.empty());
      ASSERT_TRUE(m.insert(v));
      ASSERT_FALSE(m.empty());
      ASSERT_FALSE(m.insert(v));
      ASSERT_EQ(v.second, m.at(v.first));
    }
    // insert(value_type &&value)
    {
      map_type m;
      ASSERT_TRUE(m.empty());
      ASSERT_TRUE(m.insert(value_type()));
      ASSERT_FALSE(m.insert(value_type()));
      ASSERT_EQ(value_type().second, m.at(value_type().first));
    }
    // insert(std::initializer_list<value_type> ilist)
    {
      map_type m;
      ASSERT_TRUE(m.empty());
      m.insert({
          {key_type(), mapped_type()},
      });
      ASSERT_FALSE(m.empty());
      ASSERT_EQ(mapped_type(), m.at(key_type()));
    }
    // insert(node_type &&nh)
    {
      map_type m;
      value_type v;
      ASSERT_TRUE(m.empty());
      ASSERT_TRUE(m.insert(v));
      ASSERT_FALSE(m.empty());
      node_type node = m.extract(v.first);
      ASSERT_FALSE(node.empty());
      ASSERT_TRUE(m.empty());
      ASSERT_EQ(node.mapped(), v.second);
      ASSERT_TRUE(m.insert(std::move(node)));
      ASSERT_FALSE(m.empty());
      ASSERT_EQ(v.second, m.at(v.first));
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, insert_or_assign) {
    using map_type    = TypeParam;
    using key_type    = typename map_type::key_type;
    using mapped_type = typename map_type::mapped_type;

    // insert_or_assign(const Key &k, M &&obj)
    {
      map_type m;
      key_type k    = key_type();
      mapped_type v = mapped_type();

      ASSERT_TRUE(m.empty());
      ASSERT_TRUE(m.insert_or_assign(k, v));
      ASSERT_FALSE(m.empty());
      ASSERT_FALSE(m.insert_or_assign(k, v));
      ASSERT_EQ(v, m.at(k));
    }

    // insert_or_assign(Key &&k, M &&obj)
    {
      map_type m;
      key_type k;
      mapped_type v;

      ASSERT_TRUE(m.empty());
      ASSERT_TRUE(m.insert_or_assign(std::move(k), std::move(v)));
      ASSERT_FALSE(m.empty());
      ASSERT_FALSE(m.insert_or_assign(std::move(k), std::move(v)));
      ASSERT_EQ(v, m.at(k));
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, erase) {
    using map_type    = TypeParam;
    using key_type    = typename map_type::key_type;
    using mapped_type = typename map_type::mapped_type;

    map_type m;
    ASSERT_TRUE(m.empty());
    key_type k    = key_type();
    mapped_type v = mapped_type();
    ASSERT_EQ(0, m.erase(k));
    ASSERT_TRUE(m.insert_or_assign(k, v));
    ASSERT_EQ(1, m.erase(k));
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, swap) {
    using map_type   = TypeParam;
    using key_type   = typename map_type::key_type;
    using value_type = typename map_type::value_type;

    // swap(UnorderedMap<Key, Val, Hash, Pred, Allocator> &other)
    {
      auto m1 = initialize_test_map<map_type>();
      auto m2 = initialize_test_map<map_type>();
      ASSERT_EQ(m1, m2) << "Error in test setup logic, m1 and m2 should start off equal.";
      (void) m1.erase(key_type());
      (void) m2.erase(key_type());
      ASSERT_TRUE(m1.insert(value_type()));
      ASSERT_NE(m1, m2);
      ASSERT_TRUE(m1.find(key_type()));
      ASSERT_FALSE(m2.find(key_type()));
      m1.swap(m2);
      ASSERT_NE(m1, m2);
      ASSERT_FALSE(m1.find(key_type()));
      ASSERT_TRUE(m2.find(key_type()));
    }

    // swap(internal_map_type &other)
    {
      auto m1  = initialize_test_map<map_type>();
      auto m2d = initialize_test_map<map_type>().data();
      ASSERT_EQ(m1.data(), m2d) << "Error in test setup logic, m1 and m2 should start off equal.";
      (void) m1.erase(key_type());
      ASSERT_TRUE(m1.insert(value_type()));
      ASSERT_NE(m1.data(), m2d);
      ASSERT_TRUE(m1.find(key_type()));
      ASSERT_TRUE(m2d.find(key_type()) == m2d.end());
      m1.swap(m2d);
      ASSERT_NE(m1.data(), m2d);
      ASSERT_FALSE(m1.find(key_type()));
      ASSERT_TRUE(m2d.find(key_type()) != m2d.end());
    }

    // std::swap
    {
      auto m1 = initialize_test_map<map_type>();
      auto m2 = initialize_test_map<map_type>();
      ASSERT_EQ(m1, m2) << "Error in test setup logic, m1 and m2 should start off equal.";
      (void) m1.erase(key_type());
      (void) m2.erase(key_type());
      ASSERT_TRUE(m1.insert(value_type()));
      ASSERT_NE(m1, m2);
      ASSERT_TRUE(m1.find(key_type()));
      ASSERT_FALSE(m2.find(key_type()));
      std::swap(m1, m2);
      ASSERT_NE(m1, m2);
      ASSERT_FALSE(m1.find(key_type()));
      ASSERT_TRUE(m2.find(key_type()));
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, extract) {
    using map_type    = TypeParam;
    using key_type    = typename map_type::key_type;
    using mapped_type = typename map_type::mapped_type;

    map_type m = initialize_test_map<map_type>();
    (void) m.insert_or_assign(key_type(), mapped_type());
    ASSERT_TRUE(m.find(key_type()));
    auto node = m.extract(key_type());
    ASSERT_FALSE(node.empty());
    ASSERT_EQ(mapped_type(), node.mapped());
    ASSERT_FALSE(m.find(key_type()));
    ASSERT_TRUE(m.insert(std::move(node)));
    ASSERT_TRUE(node.empty());
    ASSERT_TRUE(m.find(key_type()));
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, merge) {
    using map_type          = TypeParam;
    using internal_map_type = typename map_type::internal_map_type;
    using key_type          = typename map_type::key_type;
    using mapped_type       = typename map_type::mapped_type;
    using hasher            = typename map_type::hasher;
    using key_equal         = typename map_type::key_equal;
    using allocator_type    = typename map_type::allocator_type;
    using multimap          = std::unordered_multimap<key_type, mapped_type, hasher, key_equal, allocator_type>;

    // merge(internal_map_type &)
    {
      internal_map_type m1 = initialize_test_map<map_type>().data();
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(m1);
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      map_type m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      internal_map_type m2d = m2.data();
      m3.merge(m2d);
      ASSERT_EQ(1, m2d.size());
      ASSERT_NE(m2d.end(), m2d.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m3.find(el.first));
      }
    }
    // merge(internal_map_type &&)
    {
      internal_map_type m1 = initialize_test_map<map_type>().data();
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(std::move(m1));
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      map_type m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      internal_map_type m2d = m2.data();
      m3.merge(std::move(m2d));
      ASSERT_EQ(1, m2d.size());
      ASSERT_NE(m2d.end(), m2d.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m3.find(el.first));
      }
    }

    // merge(multimap &)
    {
      auto init = initialize_test_map<map_type>().data();
      multimap m1(init.begin(), init.end());
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(m1);
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      multimap m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_NE(m3.end(), m3.find(key_type()));
      m2.merge(m3);
      ASSERT_EQ(1, m3.size());
      ASSERT_NE(m3.end(), m3.find(key_type()));
      ASSERT_TRUE(m2.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m2.find(el.first));
      }
    }
    // merge(multimap &&)
    {
      auto init = initialize_test_map<map_type>().data();
      multimap m1(init.begin(), init.end());
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(std::move(m1));
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      multimap m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_NE(m3.end(), m3.find(key_type()));
      m2.merge(std::move(m3));
      ASSERT_EQ(1, m3.size());
      ASSERT_NE(m3.end(), m3.find(key_type()));
      ASSERT_TRUE(m2.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m2.find(el.first));
      }
    }

    // merge(map_type &)
    {
      map_type m1 = initialize_test_map<map_type>();
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(m1);
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      map_type m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      m3.merge(m2);
      ASSERT_EQ(1, m2.size());
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m3.find(el.first));
      }
    }
    // merge(map_type &&)
    {
      map_type m1 = initialize_test_map<map_type>();
      map_type m2;
      ASSERT_FALSE(m1.empty());
      ASSERT_TRUE(m2.empty());
      m2.merge(std::move(m1));
      ASSERT_TRUE(m1.empty());
      ASSERT_FALSE(m2.empty());
      (void) m2.insert({key_type(), mapped_type()});
      map_type m3 = {{key_type(), mapped_type()}};
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      m3.merge(std::move(m2));
      ASSERT_EQ(1, m2.size());
      ASSERT_TRUE(m2.find(key_type()));
      ASSERT_TRUE(m3.find(key_type()));
      for (auto const &el: initialize_test_map<map_type>().data()) {
        ASSERT_TRUE(m3.find(el.first));
      }
    }

    if constexpr (std::is_same_v<map_type, ::Concurrent::ShardedUnorderedMap<key_type, mapped_type, ::Concurrent::DefaultUnorderedMapShardCount, hasher, key_equal, allocator_type>>) {
      // Sanity check that these tests are running.
      std::cerr << "[          ] Testing merge(shard_type)\n";

      using shard_type = typename map_type::shard_type;

      // merge(shard_type &)
      {
        shard_type m1 = initialize_test_map<shard_type>();
        map_type m2;
        ASSERT_FALSE(m1.empty());
        ASSERT_TRUE(m2.empty());
        m2.merge(m1);
        ASSERT_TRUE(m1.empty());
        ASSERT_FALSE(m2.empty());
        (void) m1.insert({key_type(), mapped_type()});
        map_type m3 = {{key_type(), mapped_type()}};
        ASSERT_TRUE(m1.find(key_type()));
        ASSERT_TRUE(m3.find(key_type()));
        m3.merge(m1);
        ASSERT_EQ(1, m1.size());
        ASSERT_EQ(1, m3.size());
        ASSERT_TRUE(m1.find(key_type()));
        ASSERT_TRUE(m3.find(key_type()));
      }
      // merge(shard_type &&)
      {
        shard_type m1 = initialize_test_map<shard_type>();
        map_type m2;
        ASSERT_FALSE(m1.empty());
        ASSERT_TRUE(m2.empty());
        m2.merge(std::move(m1));
        ASSERT_TRUE(m1.empty());
        ASSERT_FALSE(m2.empty());
        (void) m1.insert({key_type(), mapped_type()});
        map_type m3 = {{key_type(), mapped_type()}};
        ASSERT_TRUE(m1.find(key_type()));
        ASSERT_TRUE(m3.find(key_type()));
        m3.merge(std::move(m1));
        ASSERT_EQ(1, m1.size());
        ASSERT_EQ(1, m3.size());
        ASSERT_TRUE(m1.find(key_type()));
        ASSERT_TRUE(m3.find(key_type()));
      }
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, at) {
    using map_type   = TypeParam;
    using value_type = typename map_type::value_type;

    // at(const Key &)
    {
      map_type m;
      auto key = value_type().first;
      try {
        (void) m.at(key);
        FAIL() << "Expected std::out_of_range.";
      } catch (std::out_of_range const &) {
      } catch (...) {
        FAIL() << "Expected std::out_of_range.";
      }
      ASSERT_TRUE(m.insert(value_type()));
      ASSERT_EQ(value_type().second, m.at(key));
    }
    // at(const Key &&)
    {
      map_type m;
      try {
        (void) m.at(value_type().first);
        FAIL() << "Expected std::out_of_range.";
      } catch (std::out_of_range const &) {
      } catch (...) {
        FAIL() << "Expected std::out_of_range.";
      }
      ASSERT_TRUE(m.insert(value_type()));
      ASSERT_EQ(value_type().second, m.at(value_type().first));
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, subscript) {
    using map_type   = TypeParam;
    using value_type = typename map_type::value_type;

    // operator[](const Key &)
    {
      map_type m;
      const auto key = value_type().first;
      const auto val = value_type().second;
      ASSERT_TRUE(m.empty());
      (void) m[key];
      ASSERT_FALSE(m.empty());
      ASSERT_EQ(1, m.size());
      ASSERT_EQ(val, m[key]);
    }
    // operator[](const Key &&)
    {
      map_type m;
      ASSERT_TRUE(m.empty());
      (void) m[value_type().first];
      ASSERT_FALSE(m.empty());
      ASSERT_EQ(1, m.size());
      ASSERT_EQ(value_type().second, m[value_type().first]);
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, count) {
    using map_type   = TypeParam;
    using value_type = typename map_type::value_type;

    map_type m;
    const auto key = value_type().first;
    ASSERT_TRUE(m.empty());
    ASSERT_EQ(0, m.count(key));
    (void) m[key];
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(1, m.size());
    ASSERT_EQ(1, m.count(key));
    (void) m.insert(value_type());
    ASSERT_EQ(1, m.size());
    ASSERT_EQ(1, m.count(key));
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, find) {
    using map_type   = TypeParam;
    using value_type = typename map_type::value_type;

    map_type m;
    const auto key = value_type().first;
    ASSERT_TRUE(m.empty());
    ASSERT_FALSE(m.find(key));
    (void) m[key];
    ASSERT_FALSE(m.empty());
    ASSERT_EQ(1, m.size());
    ASSERT_TRUE(m.find(key));
    (void) m.insert(value_type());
    ASSERT_EQ(1, m.size());
    ASSERT_TRUE(m.find(key));
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, data) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    auto mdata = m.data();
    for (auto const &[key, val]: mdata) {
      ASSERT_TRUE(m.find(key));
      ASSERT_EQ(val, m.at(key));
    }
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, load_factor) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    ASSERT_NE(0, m.load_factor());
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, max_load_factor) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    m.max_load_factor(0.54321);
    ASSERT_NEAR(0.54321, m.max_load_factor(), 0.001);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, rehash) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    m.rehash(100);
  }

  TYPED_TEST_P(CommonConcurrentUnorderedMapTests, reserve) {
    using map_type = TypeParam;

    map_type m = initialize_test_map<map_type>();
    m.reserve(100);
  }

  REGISTER_TYPED_TEST_SUITE_P(CommonConcurrentUnorderedMapTests, // Comments so clang-format keeps
                              DefaultConstructor,                // these lines broken.
                              CopyConstructor,                   //
                              MoveConstructor,                   //
                              CopyAssignment,                    //
                              MoveAssignment,                    //
                              get_allocator,                     //
                              empty,                             //
                              size,                              //
                              clear,                             //
                              insert,                            //
                              insert_or_assign,                  //
                              erase,                             //
                              swap,                              //
                              extract,                           //
                              merge,                             //
                              at,                                //
                              subscript,                         //
                              count,                             //
                              find,                              //
                              data,                              //
                              load_factor,                       //
                              max_load_factor,                   //
                              rehash,                            //
                              reserve                            //
  );

  using Types = ::testing::Types<                                                              // Comments so clang-format keeps
      UnorderedMap<std::string, uint32_t>,                                                     // these lines broken.
      UnorderedMap<std::string, std::string>,                                                  //
      UnorderedMap<std::string, float>,                                                        //
      UnorderedMap<int32_t, uint64_t>,                                                         //
      UnorderedMap<int64_t, size_t>,                                                           //
      UnorderedMap<int32_t, std::string>,                                                      //
      UnorderedMap<int64_t, std::string>,                                                      //
      UnorderedMap<Foo, int16_t, FooHash>,                                                     //
      UnorderedMap<int16_t, Foo>,                                                              //
      ShardedUnorderedMap<std::string, uint32_t>,                                              //
      ShardedUnorderedMap<std::string, std::string>,                                           //
      ShardedUnorderedMap<std::string, float>,                                                 //
      ShardedUnorderedMap<int32_t, uint64_t>,                                                  //
      ShardedUnorderedMap<int64_t, size_t>,                                                    //
      ShardedUnorderedMap<int32_t, std::string>,                                               //
      ShardedUnorderedMap<int64_t, std::string>,                                               //
      ShardedUnorderedMap<Foo, int16_t, ::Concurrent::DefaultUnorderedMapShardCount, FooHash>, //
      ShardedUnorderedMap<int16_t, Foo>>;                                                      //

  INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, CommonConcurrentUnorderedMapTests, Types);

  TEST_F(UnshardedConcurrentUnorderedMapTests, IListConstructor) {
    UnorderedMap<std::string, std::string> umap{
        {"foo", "qux"},
        {"bar", "quux"},
        {"baz", "quuux"},
    };

    ASSERT_FALSE(umap.empty());
    ASSERT_EQ(3, umap.size());
    ASSERT_EQ("qux", umap.at("foo"));
    ASSERT_EQ("quux", umap.at("bar"));
    ASSERT_EQ("quuux", umap.at("baz"));
  }
  TEST_F(ShardedConcurrentUnorderedMapTests, IListConstructor) {
    ShardedUnorderedMap<std::string, std::string> umap{
        {"foo", "qux"},
        {"bar", "quux"},
        {"baz", "quuux"},
    };

    ASSERT_FALSE(umap.empty());
    ASSERT_EQ(3, umap.size());
    ASSERT_EQ("qux", umap.at("foo"));
    ASSERT_EQ("quux", umap.at("bar"));
    ASSERT_EQ("quuux", umap.at("baz"));
  }

  TEST_F(UnshardedConcurrentUnorderedMapTests, IListAssignment) {
    UnorderedMap<std::string, std::string> umap = {
        {"foo", "qux"},
        {"bar", "quux"},
        {"baz", "quuux"},
    };

    ASSERT_FALSE(umap.empty());
    ASSERT_EQ(3, umap.size());
    ASSERT_EQ("qux", umap.at("foo"));
    ASSERT_EQ("quux", umap.at("bar"));
    ASSERT_EQ("quuux", umap.at("baz"));
  }
  TEST_F(ShardedConcurrentUnorderedMapTests, IListAssignment) {
    ShardedUnorderedMap<std::string, std::string> umap = {
        {"foo", "qux"},
        {"bar", "quux"},
        {"baz", "quuux"},
    };

    ASSERT_FALSE(umap.empty());
    ASSERT_EQ(3, umap.size());
    ASSERT_EQ("qux", umap.at("foo"));
    ASSERT_EQ("quux", umap.at("bar"));
    ASSERT_EQ("quuux", umap.at("baz"));
  }

  TEST_F(UnshardedConcurrentUnorderedMapTests, max_size) {
    UnorderedMap<std::string, std::string> umap;
    ASSERT_LT(0, umap.max_size());
  }

  TEST_F(UnshardedConcurrentUnorderedMapTests, emplace) {
    UnorderedMap<std::string, std::string> umap;
    ASSERT_TRUE(umap.empty());
    ASSERT_TRUE(umap.emplace("foo", "bar"));
    ASSERT_FALSE(umap.emplace("foo", "baz"));
    ASSERT_EQ("bar", umap["foo"]);
  }

  TEST_F(UnshardedConcurrentUnorderedMapTests, try_emplace) {
    // try_emplace(const Key &k, Args &&...args)
    {
      UnorderedMap<std::string, Foo> umap;
      ASSERT_TRUE(umap.empty());
      std::string key  = "foo";
      int val1         = 1;
      std::string val2 = "bar";
      ASSERT_TRUE(umap.try_emplace(key, val1, val2));
      ASSERT_FALSE(umap.try_emplace(key, val1, val2));
      ASSERT_EQ(Foo(val1, val2), umap["foo"]);
    }

    // try_emplace(Key &&k, Args &&...args)
    {
      UnorderedMap<std::string, Foo> umap;
      ASSERT_TRUE(umap.empty());
      std::string key  = "foo";
      int val1         = 1;
      std::string val2 = "bar";
      ASSERT_TRUE(umap.try_emplace(std::move(key), std::move(val1), std::move(val2)));
      ASSERT_FALSE(umap.try_emplace(std::move(key), std::move(val1), std::move(val2)));
      ASSERT_EQ(Foo(val1, val2), umap["foo"]);
    }
  }

  TEST_F(UnshardedConcurrentUnorderedMapTests, bucket_count) {
    using map_type = ::Concurrent::UnorderedMap<std::string, uint32_t>;

    map_type m = initialize_test_map<map_type>();
    ASSERT_LT(0, m.bucket_count());
  }
  TEST_F(UnshardedConcurrentUnorderedMapTests, max_bucket_count) {
    using map_type = ::Concurrent::UnorderedMap<std::string, uint32_t>;

    map_type m = initialize_test_map<map_type>();
    ASSERT_LT(0, m.max_bucket_count());
  }
  TEST_F(UnshardedConcurrentUnorderedMapTests, bucket_size) {
    using map_type = ::Concurrent::UnorderedMap<std::string, uint32_t>;

    map_type m = initialize_test_map<map_type>();
    m.insert({"foo", 1});
    ASSERT_LE(1, m.bucket_size(m.bucket("foo")));
  }
  TEST_F(UnshardedConcurrentUnorderedMapTests, bucket) {
    using map_type = ::Concurrent::UnorderedMap<std::string, uint32_t>;

    map_type m = initialize_test_map<map_type>();
    m.insert({"foo", 1});
    ASSERT_LE(0, m.bucket("foo"));
  }

  TEST_F(ShardedConcurrentUnorderedMapTests, shard_count) {
    ShardedUnorderedMap<std::string, std::string, ::Concurrent::DefaultUnorderedMapShardCount> umap{
        {"foo", "qux"},
        {"bar", "quux"},
        {"baz", "quuux"},
    };

    ASSERT_EQ(::Concurrent::DefaultUnorderedMapShardCount, umap.shard_count());
  }

  TEST_F(ShardedConcurrentUnorderedMapTests, shard_load_factor) {
    ShardedUnorderedMap<std::string, std::string, ::Concurrent::DefaultUnorderedMapShardCount> umap;
    for (uint32_t i = 0; i < ::Concurrent::DefaultUnorderedMapShardCount; ++i) {
      ASSERT_NEAR(0, umap.shard_load_factor(i), 0.0001);
    }
  }
} // anonymous namespace