#include <ShardedUnorderedConcurrentMap.h>
#include <UnorderedConcurrentMap.h>
#include <gtest/gtest.h>
#include <string>

using ::Concurrent::ShardedUnorderedMap;
using ::Concurrent::UnorderedMap;

// Common test cases for both
// ::Concurrent::ShardedUnorderedMap, and
// ::Concurrent::UnorderedMap.
template <typename T>
class CommonConcurrentUnorderedMapTests : public ::testing::Test {
protected:
  // void SetUp() override;
  // void TearDown() override;
};

// ------------------------ Typed Tests ------------------------ //

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
    umap[key_type()];
    FAIL() << "Expected std::out_of_range.";
  } catch (std::out_of_range const &) {
  } catch (...) {
    FAIL() << "Expected std::out_of_range.";
  }

  EXPECT_EQ(0, umap.erase(key_type())) << "Expected no elements to be removed.";
  EXPECT_EQ(0, umap.count(key_type())) << "Expected no elements to be found.";
  EXPECT_FALSE(umap.find(key_type())) << "Expected no elements to be found.";

  auto data = umap.data();
  ASSERT_EQ(expected_size, data.size());
  ASSERT_TRUE(data.empty());

  EXPECT_EQ(map_type(), umap);
  EXPECT_FALSE(umap != map_type());
}

REGISTER_TYPED_TEST_SUITE_P(CommonConcurrentUnorderedMapTests, DefaultConstructor);
using Types = ::testing::Types<                    // comments so clang-format keeps
    UnorderedMap<std::string, uint32_t>,           // these lines broken.
    UnorderedMap<int32_t, uint64_t>,               //
    UnorderedMap<int64_t, size_t>,                 //
    UnorderedMap<std::string, std::string>,        //
    UnorderedMap<int32_t, std::string>,            //
    UnorderedMap<int64_t, std::string>,            //
    ShardedUnorderedMap<std::string, uint32_t>,    //
    ShardedUnorderedMap<int32_t, uint64_t>,        //
    ShardedUnorderedMap<int64_t, size_t>,          //
    ShardedUnorderedMap<std::string, std::string>, //
    ShardedUnorderedMap<int32_t, std::string>,     //
    ShardedUnorderedMap<int64_t, std::string>>;    //
INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, CommonConcurrentUnorderedMapTests, Types);

// ----------------------- Untyped Tests ----------------------- //