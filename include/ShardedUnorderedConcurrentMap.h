#ifndef SHARDED_UNORDERED_CONCURRENT_MAP
#define SHARDED_UNORDERED_CONCURRENT_MAP

#include <UnorderedConcurrentMap.h>

namespace Concurrent {
  constexpr uint32_t DefaultUnorderedMapShardCount = 32;

  template <class Key, class Val, uint32_t ShardCount = DefaultUnorderedMapShardCount, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, Val>>>
  class ShardedUnorderedMap {
  public:
    // ------------------------------ Member types ------------------------------ //
    using shard_type           = UnorderedMap<Key, Val, Hash, Pred, Allocator>;
    using internal_map_type    = typename shard_type::internal_map_type;
    using key_type             = typename shard_type::key_type;
    using mapped_type          = typename shard_type::mapped_type;
    using value_type           = typename shard_type::value_type;
    using size_type            = typename shard_type::size_type;
    using difference_type      = typename shard_type::difference_type;
    using hasher               = typename shard_type::hasher;
    using key_equal            = typename shard_type::key_equal;
    using allocator_type       = typename shard_type::allocator_type;
    using reference            = typename shard_type::reference;
    using const_reference      = typename shard_type::const_reference;
    using pointer              = typename shard_type::pointer;
    using const_pointer        = typename shard_type::const_pointer;
    using iterator             = typename shard_type::iterator;
    using const_iterator       = typename shard_type::const_iterator;
    using local_iterator       = typename shard_type::local_iterator;
    using const_local_iterator = typename shard_type::const_local_iterator;
    using node_type            = typename shard_type::node_type;

    // ------------------------------ Constructors ------------------------------ //
    ShardedUnorderedMap() { validate_shard_count(); }
    ShardedUnorderedMap(const ShardedUnorderedMap &other) : m_shards(other.m_shards) { validate_shard_count(); }
    ShardedUnorderedMap(ShardedUnorderedMap &&other) : m_shards(other.m_shards) { validate_shard_count(); }
    ShardedUnorderedMap(std::initializer_list<value_type> init) {
      validate_shard_count();
      for (auto const &el: init) {
        (void) insert(el);
      }
    }

    ShardedUnorderedMap &operator=(const ShardedUnorderedMap &other) { return ShardedUnorderedMap(other); }
    ShardedUnorderedMap &operator=(ShardedUnorderedMap &&other) noexcept { return ShardedUnorderedMap(other); }
    ShardedUnorderedMap &operator=(std::initializer_list<value_type> ilist) { return ShardedUnorderedMap(ilist); }

    ~ShardedUnorderedMap() = default;

    allocator_type get_allocator() const { return m_shards.a(0).get_allocator(); }

    // -------------------------------- Capacity -------------------------------- //
    bool empty() const noexcept {
      for (auto const &s: m_shards) {
        if (!s.empty()) return false;
      }
      return true;
    }

    size_type size() const noexcept {
      size_type size = 0;
      for (auto &s: m_shards) {
        size += s.size();
      }
      return size;
    }

    // ------------------------------- Modifiers -------------------------------- //
    bool insert(const value_type &value) { return get_shard(value.first).insert(value); }

    // ------------------------------ Accessors --------------------------------- //
    // Returns a non-thread-safe copy of the underlying data.
    internal_map_type data() {
      internal_map_type m;
      for (auto &s: m_shards) {
        m.merge(s.data());
      }
      return m;
    }

    // --------------------------- Bucket Interface ----------------------------- //

    // ------------------------------ Hash Policy ------------------------------- //

    // ------------------------------- Observers -------------------------------- //
    hasher hash_function() const { return m_shards.at(0).hash_function(); }

    key_equal key_eq() const { return m_shards.at(0).key_eq(); }

  private:
    std::array<shard_type, ShardCount> m_shards{};

    void validate_shard_count() { static_assert(ShardCount != 0, "ShardCount template parameter must be non-zero."); }

    uint32_t get_shard_idx(Key const &key) { return hash_function()(key) % ShardCount; }
    uint32_t get_shard_idx(Key const &&key) { return hash_function()(key) % ShardCount; }
    shard_type &get_shard(Key const &key) { return m_shards.at(get_shard_idx(key)); }
    shard_type &get_shard(Key const &&key) { return m_shards.at(get_shard_idx(key)); }
  };

} // namespace Concurrent

#endif // SHARDED_UNORDERED_CONCURRENT_MAP