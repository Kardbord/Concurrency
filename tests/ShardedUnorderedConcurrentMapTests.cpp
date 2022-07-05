#include <ShardedUnorderedConcurrentMap.h>
#include <gtest/gtest.h>
#include <string>

using ::Concurrent::ShardedUnorderedMap;

template <typename T>
class ShardedUnorderedConcurrentMapTest : public ::testing::Test {
protected:
  // void SetUp() override;
  // void TearDown() override;
};

// ------------------------ Typed Tests ------------------------ //

TYPED_TEST_SUITE_P(ShardedUnorderedConcurrentMapTest);
TYPED_TEST_P(ShardedUnorderedConcurrentMapTest, DefaultConstructor) {
  using map_type = ShardedUnorderedMap<TypeParam, TypeParam>;
  map_type umap;
  constexpr typename map_type::size_type expected_size = 0;
  ASSERT_EQ(expected_size, umap.size());
  ASSERT_TRUE(umap.empty());
  try {
    umap.at(TypeParam());
  } catch (std::out_of_range const &) {
  } catch (...) {
    FAIL() << "Expected std::out_of_range.";
  }
}

REGISTER_TYPED_TEST_SUITE_P(ShardedUnorderedConcurrentMapTest, DefaultConstructor);
using Types = ::testing::Types<std::string, uint32_t, int32_t, uint64_t, int64_t, size_t>;
INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, ShardedUnorderedConcurrentMapTest, Types);

// ----------------------- Untyped Tests ----------------------- //