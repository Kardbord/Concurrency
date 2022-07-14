#ifndef SHARDED_UNORDERED_CONCURRENT_MAP
#define SHARDED_UNORDERED_CONCURRENT_MAP

#include <UnorderedConcurrentMap.h>

namespace Concurrent {
  constexpr uint32_t DefaultUnorderedMapShardCount = 32;

  // This class provides a sharded, thread-safe, unordered map with most of the same
  // functionality as std::unordered_map. However, iterator access has been removed in order
  // to preserve thread-safety. No direct access to begin() or end() iterators is provided.
  // Iterators have also been removed from the return type of any function which typically
  // includes them.
  //
  // Aside from the above, functions which behave differently than their std::unordered_map
  // counterpart of the same name are documented with comments, as are functions that
  // do not exist for std::unordered_map.
  //
  // https://en.cppreference.com/w/cpp/container/unordered_map
  // TODO: Support emplace() and try_emplace().
  template <class Key, class Val, uint32_t ShardCount = DefaultUnorderedMapShardCount, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, Val>>>
  class ShardedUnorderedMap {
  public:
    // ------------------------------ Member types ------------------------------ //
    using self_type            = ShardedUnorderedMap<Key, Val, ShardCount, Hash, Pred, Allocator>;
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
    ShardedUnorderedMap(const ShardedUnorderedMap &other) {
      validate_shard_count();
      for (uint32_t i = 0; i < ShardCount; ++i) {
        m_shards[i] = other.m_shards[i];
      }
    }
    ShardedUnorderedMap(ShardedUnorderedMap &&other) {
      validate_shard_count();
      for (uint32_t i = 0; i < ShardCount; ++i) {
        m_shards[i] = other.m_shards[i];
      }
    }
    ShardedUnorderedMap(std::initializer_list<value_type> ilist) {
      validate_shard_count();
      insert(ilist);
    }

    ShardedUnorderedMap &operator=(const ShardedUnorderedMap &other) {
      validate_shard_count();
      for (uint32_t i = 0; i < ShardCount; ++i) {
        m_shards[i] = other.m_shards[i];
      }
      return *this;
    }
    ShardedUnorderedMap &operator=(ShardedUnorderedMap &&other) noexcept {
      validate_shard_count();
      for (uint32_t i = 0; i < ShardCount; ++i) {
        m_shards[i] = other.m_shards[i];
      }
      return *this;
    }
    ShardedUnorderedMap &operator=(std::initializer_list<value_type> ilist) {
      validate_shard_count();
      this->insert(ilist);
      return *this;
    }

    ~ShardedUnorderedMap() = default;

    allocator_type get_allocator() const { return m_shards.at(0).get_allocator(); }

    // -------------------------------- Capacity -------------------------------- //
    bool empty() const noexcept {
      for (auto &s: m_shards) {
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

    void clear() noexcept {
      for (auto &s: m_shards) {
        s.clear();
      }
    }

    bool insert(const value_type &value) { return get_mutable_shard(value.first).insert(value); }
    bool insert(value_type &&value) { return get_mutable_shard(value.first).insert(value); }
    void insert(std::initializer_list<value_type> ilist) {
      for (auto const &el: ilist) {
        (void) insert(el);
      }
    }
    bool insert(node_type &&nh) { return get_mutable_shard(nh.key()).insert(std::move(nh)); }

    template <class M>
    bool insert_or_assign(const Key &k, M &&obj) {
      return get_mutable_shard(k).insert_or_assign(k, obj);
    }
    template <class M>
    bool insert_or_assign(Key &&k, M &&obj) {
      return get_mutable_shard(k).insert_or_assign(k, obj);
    }

    size_type erase(const Key &key) { return get_mutable_shard(key).erase(key); }

    void swap(ShardedUnorderedMap<Key, Val, ShardCount, Hash, Pred, Allocator> &other) noexcept {
      for (uint32_t i = 0; i < ShardCount; ++i) {
        this->m_shards[i].swap(other.m_shards[i]);
      }
    }

    void swap(internal_map_type &other) noexcept {
      internal_map_type tmp = other;
      other.clear();
      for (auto &s: m_shards) {
        other.merge(s.data());
      }
      this->clear();
      for (auto const &el: tmp) {
        this->insert(el);
      }
    }

    node_type extract(const Key &k) { return get_mutable_shard(k).extract(k); }

    void merge(internal_map_type &source) {
      auto tmp = source;
      for (auto const &el: tmp) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(internal_map_type &&source) {
      auto tmp = source;
      for (auto const &el: tmp) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(std::unordered_multimap<Key, Val, Hash, Pred, Allocator> &source) {
      auto tmp = source;
      for (auto const &el: tmp) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(std::unordered_multimap<Key, Val, Hash, Pred, Allocator> &&source) {
      auto tmp = source;
      for (auto const &el: tmp) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(UnorderedMap<Key, Val, Hash, Pred, Allocator> &source) {
      for (auto const &el: source.data()) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(UnorderedMap<Key, Val, Hash, Pred, Allocator> &&source) {
      for (auto const &el: source.data()) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(ShardedUnorderedMap<Key, Val, ShardCount, Hash, Pred, Allocator> &source) {
      for (auto const &el: source.data()) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }
    void merge(ShardedUnorderedMap<Key, Val, ShardCount, Hash, Pred, Allocator> &&source) {
      for (auto const &el: source.data()) {
        if (find(el.first)) continue;
        (void) insert(std::move(source.extract(el.first)));
      }
    }

    // ------------------------------ Accessors --------------------------------- //
    // Returns a copy of the element mapped to
    // the provided key. Does bounds checking.
    Val at(const Key &key) const { return get_shard(key).at(key); }
    // Returns a copy of the element mapped to
    // the provided key. Does bounds checking.
    Val at(const Key &&key) const { return get_shard(key).at(key); }

    // Returns a copy of the element mapped to
    // the provided key. If no element is present,
    // a new one is default constructed.
    Val operator[](const Key &key) { return get_mutable_shard(key)[key]; }
    // Returns a copy of the element mapped to
    // the provided key. If no element is present,
    // a new one is default constructed.
    Val operator[](Key &&key) { return get_mutable_shard(key)[key]; }

    size_type count(const Key &key) const { return get_shard(key).count(key); }

    // Returns a bool indicating whether or not the
    // provided key is present in the map.
    bool find(const Key &key) const { return get_shard(key).find(key); }

    // Returns a copy of the data in each
    // shard as a single non-thread-safe unordered_map.
    internal_map_type data() const {
      internal_map_type m;
      for (auto &s: m_shards) {
        m.merge(s.data());
      }
      return m;
    }

    // ------------------------------ Hash Policy ------------------------------- //
    uint32_t shard_count() const noexcept { return ShardCount; }

    // Averaged load factor across all shards.
    float load_factor() const {
      float lf = 0;
      for (auto &s: m_shards) {
        lf += s.load_factor();
      }
      return lf / ShardCount;
    }

    // Returns the load factor for a given shard.
    // If shard_idx is greater than the number of shards,
    // -1.0 is returned.
    float shard_load_factor(uint32_t const shard_idx) const {
      if (shard_idx >= ShardCount) {
        return -1.0;
      }
      return m_shards.at(shard_idx).load_factor();
    }

    // Returns the current maximum load factor
    // allowed for all shards.
    float max_load_factor() const { return m_shards.at(0).max_load_factor(); }

    // Sets the maximum load factor allowed
    // for all shards.
    void max_load_factor(float ml) {
      for (auto &s: m_shards) {
        s.max_load_factor(ml);
      }
    }

    // For each shard, reserves at least the specified number of buckets
    // and regenerates the hash table.
    void rehash(size_type count) {
      for (auto &s: m_shards) {
        s.rehash(count);
      }
    }

    // For each shard, reserves space for at least the specified number of
    // elements and regenerates the hash table.
    void reserve(size_type count) {
      for (auto &s: m_shards) {
        s.reserve(count);
      }
    }

    // ------------------------------- Observers -------------------------------- //
    hasher hash_function() const { return m_shards.at(0).hash_function(); }

    key_equal key_eq() const { return m_shards.at(0).key_eq(); }

  private:
    std::array<shard_type, ShardCount> m_shards{};

    void validate_shard_count() const { static_assert(ShardCount != 0, "ShardCount template parameter must be non-zero."); }

    uint32_t get_shard_idx(Key const &key) const { return hash_function()(key) % ShardCount; }
    uint32_t get_shard_idx(Key const &&key) const { return hash_function()(key) % ShardCount; }
    shard_type &get_mutable_shard(Key const &key) { return m_shards.at(get_shard_idx(key)); }
    shard_type &get_mutable_shard(Key const &&key) { return m_shards.at(get_shard_idx(key)); }
    const shard_type &get_shard(Key const &key) const { return m_shards.at(get_shard_idx(key)); }
    const shard_type &get_shard(Key const &&key) const { return m_shards.at(get_shard_idx(key)); }
  };

  template <class Key, class T, uint32_t ShardCount, class Hash, class KeyEqual, class Alloc>
  bool operator==(const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &rhs) {
    return lhs.data() == rhs.data();
  }

  template <class Key, class T, uint32_t ShardCount, class Hash, class KeyEqual, class Alloc>
  bool operator!=(const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &rhs) {
    return !(lhs == rhs);
  }

  template <class Key, class T, uint32_t ShardCount, class Hash, class KeyEqual, class Alloc>
  bool operator==(const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &&rhs) {
    return lhs.data() == rhs.data();
  }

  template <class Key, class T, uint32_t ShardCount, class Hash, class KeyEqual, class Alloc>
  bool operator!=(const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &&rhs) {
    return !(lhs == rhs);
  }

  // Specializes the std::swap algorithm for ::Concurrent::ShardedUnorderedMap. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
  template <class Key, class T, uint32_t ShardCount, class Hash, class KeyEqual, class Alloc>
  void swap(::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &lhs, ::Concurrent::ShardedUnorderedMap<Key, T, ShardCount, Hash, KeyEqual, Alloc> &rhs) noexcept {
    lhs.swap(rhs);
  }

} // namespace Concurrent
#endif // SHARDED_UNORDERED_CONCURRENT_MAP