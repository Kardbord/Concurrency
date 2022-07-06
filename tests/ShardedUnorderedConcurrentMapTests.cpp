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
    FAIL() << "Expected std::out_of_range.";
  } catch (std::out_of_range const &) {
  } catch (...) {
    FAIL() << "Expected std::out_of_range.";
  }

  try {
    umap[TypeParam()];
    FAIL() << "Expected std::out_of_range.";
  } catch (std::out_of_range const &) {
  } catch (...) {
    FAIL() << "Expected std::out_of_range.";
  }

  EXPECT_EQ(0, umap.erase(TypeParam())) << "Expected no elements to be removed.";
  EXPECT_EQ(0, umap.count(TypeParam())) << "Expected no elements to be found.";
  EXPECT_FALSE(umap.find(TypeParam()));

  auto data = umap.data();
  ASSERT_EQ(expected_size, data.size());
  ASSERT_TRUE(data.empty());

  EXPECT_EQ(map_type(), umap);
  EXPECT_FALSE(umap != map_type());
}

REGISTER_TYPED_TEST_SUITE_P(ShardedUnorderedConcurrentMapTest, DefaultConstructor);
using Types = ::testing::Types<std::string, uint32_t, int32_t, uint64_t, int64_t, size_t>;
INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, ShardedUnorderedConcurrentMapTest, Types);

// ----------------------- Untyped Tests ----------------------- //