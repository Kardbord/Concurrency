#include <UnorderedConcurrentMap.h>
#include <gtest/gtest.h>
#include <string>

using ::Concurrent::UnorderedMap;

template <typename T>
class UnorderedConcurrentMapTest : public ::testing::Test {
protected:
  // void SetUp() override;
  // void TearDown() override;
};

// ------------------------ Typed Tests ------------------------ //

TYPED_TEST_SUITE_P(UnorderedConcurrentMapTest);
TYPED_TEST_P(UnorderedConcurrentMapTest, DefaultConstructor) {
  using map_type = UnorderedMap<TypeParam, TypeParam>;
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

REGISTER_TYPED_TEST_SUITE_P(UnorderedConcurrentMapTest, DefaultConstructor);
using Types = ::testing::Types<std::string, uint32_t, int32_t, uint64_t, int64_t, size_t>;
INSTANTIATE_TYPED_TEST_SUITE_P(TypedTests, UnorderedConcurrentMapTest, Types);

// ----------------------- Untyped Tests ----------------------- //