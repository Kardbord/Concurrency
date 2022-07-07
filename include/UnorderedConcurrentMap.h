#ifndef UNORDERED_CONCURRENT_MAP_H
#define UNORDERED_CONCURRENT_MAP_H

#include <shared_mutex>
#include <unordered_map>

namespace Concurrent {

  // This class provides a thread-safe unordered map with most of the same functionality as
  // std::unordered_map. However, iterator access has been removed in order to preserve
  // thread-safety. No direct access to begin() or end() iterators is provided. Iterators
  // have also been removed from the return type of any function which typically includes
  // them.
  //
  // Aside from the above, functions which behave differently than their std::unordered_map
  // counterpart of the same name are documented with comments, as are functions that
  // do not exist for std::unordered_map.
  //
  // TODO: Write unit tests.
  // TODO: Benchmark.
  //
  // https://en.cppreference.com/w/cpp/container/unordered_map
  template <class Key, class Val, class Hash = std::hash<Key>, class Pred = std::equal_to<Key>, class Allocator = std::allocator<std::pair<const Key, Val>>>
  class UnorderedMap {
  public:
    // ------------------------------ Member types ------------------------------ //
    using mutex_type           = std::shared_mutex;
    using read_lock            = std::shared_lock<mutex_type>;
    using write_lock           = std::unique_lock<mutex_type>;
    using internal_map_type    = std::unordered_map<Key, Val, Hash, Pred, Allocator>;
    using key_type             = typename internal_map_type::key_type;
    using mapped_type          = typename internal_map_type::mapped_type;
    using value_type           = typename internal_map_type::value_type;
    using size_type            = typename internal_map_type::size_type;
    using difference_type      = typename internal_map_type::difference_type;
    using hasher               = typename internal_map_type::hasher;
    using key_equal            = typename internal_map_type::key_equal;
    using allocator_type       = typename internal_map_type::allocator_type;
    using reference            = typename internal_map_type::reference;
    using const_reference      = typename internal_map_type::const_reference;
    using pointer              = typename internal_map_type::pointer;
    using const_pointer        = typename internal_map_type::const_pointer;
    using iterator             = typename internal_map_type::iterator;
    using const_iterator       = typename internal_map_type::const_iterator;
    using local_iterator       = typename internal_map_type::local_iterator;
    using const_local_iterator = typename internal_map_type::const_local_iterator;
    using node_type            = typename internal_map_type::node_type;

    // This member type intentionally excluded, as it is not used in this implementation.
    // using insert_return_type   = typename internal_map_type::insert_return_type;

    // ------------------------------ Constructors ------------------------------ //
    UnorderedMap() = default;
    UnorderedMap(const UnorderedMap &other) {
      auto lock = lock_for_writing();
      m_map     = std::move(other.data());
    }
    UnorderedMap(UnorderedMap &&other) {
      auto lock = lock_for_writing();
      m_map     = std::move(other.data());
    }
    UnorderedMap(std::initializer_list<value_type> ilist) { insert(ilist); }

    UnorderedMap &operator=(const UnorderedMap &other) {
      auto lock   = lock_for_writing();
      this->m_map = other.data();
      return *this;
    }
    UnorderedMap &operator=(UnorderedMap &&other) noexcept {
      auto lock   = lock_for_writing();
      this->m_map = std::move(other.data());
      return *this;
    }
    UnorderedMap &operator=(std::initializer_list<value_type> ilist) {
      this->insert(ilist);
      return *this;
    }

    ~UnorderedMap() = default;

    allocator_type get_allocator() const { return m_map.get_allocator(); }

    // ------------------------------- Iterators -------------------------------- //
    /*
    begin(), end(), cbegin(), and cend() iterators are not supported due to the footgun they present
    to concurrent access.
    */

    // -------------------------------- Capacity -------------------------------- //
    bool empty() const noexcept {
      auto l = lock_for_reading();
      return m_map.empty();
    }

    size_type size() const noexcept {
      auto l = lock_for_reading();
      return m_map.size();
    }

    size_type max_size() const noexcept { return m_map.max_size(); }

    // ------------------------------- Modifiers -------------------------------- //

    void clear() noexcept {
      auto lock = lock_for_writing();
      m_map.clear();
    }

    bool insert(const value_type &value) {
      auto lock = lock_for_writing();
      return m_map.insert(value).second;
    }
    bool insert(value_type &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(value).second;
    }
    template <class P>
    bool insert(P &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(value).second;
    }
    void insert(std::initializer_list<value_type> ilist) {
      auto lock = lock_for_writing();
      (void) m_map.insert(ilist);
    }
    bool insert(node_type &&nh) {
      auto lock = lock_for_writing();
      return m_map.insert(std::move(nh)).inserted;
    }

    template <class M>
    bool insert_or_assign(const Key &k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(k, obj).second;
    }
    template <class M>
    bool insert_or_assign(Key &&k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(k, obj).second;
    }

    template <class... Args>
    bool emplace(Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.emplace(args...).second;
    }

    template <class... Args>
    bool try_emplace(const Key &k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(k, args...).second;
    }
    template <class... Args>
    bool try_emplace(Key &&k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(k, args...).second;
    }

    size_type erase(const Key &key) {
      auto lock = lock_for_writing();
      return m_map.erase(key);
    }

    void swap(UnorderedMap<Key, Val, Hash, Pred, Allocator> &other) noexcept {
      auto lhs_lock = this->lock_for_writing();
      auto rhs_lock = other.lock_for_writing();
      this->m_map.swap(other.m_map);
    }

    void swap(internal_map_type &other) noexcept {
      auto lock = lock_for_writing();
      m_map.swap(other);
    }

    node_type extract(const Key &k) {
      auto lock = lock_for_writing();
      return m_map.extract(k);
    }

    void merge(internal_map_type &source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    void merge(internal_map_type &&source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    void merge(std::unordered_multimap<Key, Val, Hash, Pred, Allocator> &source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    void merge(std::unordered_multimap<Key, Val, Hash, Pred, Allocator> &&source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
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

    // ------------------------------ Accessors --------------------------------- //
    // Returns a copy of the element mapped to
    // the provided key. Does bounds checking.
    Val at(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.at(key);
    }
    // Returns a copy of the element mapped to
    // the provided key. Does bounds checking.
    Val at(const Key &&key) const {
      auto lock = lock_for_reading();
      return m_map.at(key);
    }

    // Returns a copy of the element mapped to
    // the provided key. If no element is present,
    // a new one is default constructed.
    Val operator[](const Key &key) {
      if (this->find(key)) return this->at(key);
      auto lock = lock_for_writing();
      return m_map[key];
    }
    // Returns a copy of the element mapped to
    // the provided key. If no element is present,
    // a new one is default constructed.
    Val operator[](Key &&key) {
      if (this->find(key)) return this->at(key);
      auto lock = lock_for_writing();
      return m_map[key];
    }

    size_type count(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.count(key);
    }

    // Returns a bool indicating whether or not the
    // provided key is present in the map.
    bool find(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.find(key) != m_map.end();
    }

    // Returns a non-thread-safe copy of the underlying map.
    internal_map_type data() const {
      auto lock = lock_for_reading();
      return m_map;
    }

    // --------------------------- Bucket Interface ----------------------------- //
    size_type bucket_count() const {
      auto lock = lock_for_reading();
      return m_map.bucket_count();
    }

    size_type max_bucket_count() const { return m_map.max_bucket_count(); }

    size_type bucket_size(size_type n) const {
      auto lock = lock_for_reading();
      return m_map.bucket_size(n);
    }

    size_type bucket(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.bucket(key);
    }

    // ------------------------------ Hash Policy ------------------------------- //
    float load_factor() const {
      auto lock = lock_for_reading();
      return m_map.load_factor();
    }

    float max_load_factor() const {
      auto lock = lock_for_reading();
      return m_map.max_load_factor();
    }

    void max_load_factor(float ml) {
      auto lock = lock_for_writing();
      m_map.max_load_factor(ml);
    }

    void rehash(size_type count) {
      auto lock = lock_for_writing();
      m_map.rehash(count);
    }

    void reserve(size_type count) {
      auto lock = lock_for_writing();
      m_map.reserve(count);
    }

    // ------------------------------- Observers -------------------------------- //
    hasher hash_function() const { return m_map.hash_function(); }

    key_equal key_eq() const { return m_map.key_eq(); }

  private:
    // Returns a locked read_lock that prevents concurrent write access to
    // the underlying map.
    read_lock lock_for_reading() const { return read_lock(m_mutex); }

    // Returns a locked write_lock that prevents concurrent access to the
    // underlying map.
    write_lock lock_for_writing() const { return write_lock(m_mutex); }

    mutable mutex_type m_mutex{};
    internal_map_type m_map{};
  };

  template <class Key, class T, class Hash, class KeyEqual, class Alloc>
  bool operator==(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &rhs) {
    return lhs.data() == rhs.data();
  }

  template <class Key, class T, class Hash, class KeyEqual, class Alloc>
  bool operator!=(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &rhs) {
    return !(lhs == rhs);
  }

  template <class Key, class T, class Hash, class KeyEqual, class Alloc>
  bool operator==(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &&rhs) {
    return lhs.data() == rhs.data();
  }

  template <class Key, class T, class Hash, class KeyEqual, class Alloc>
  bool operator!=(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &&rhs) {
    return !(lhs == rhs);
  }

  // Specializes the std::swap algorithm for ::Concurrent::UnorderedMap. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
  template <class Key, class T, class Hash, class KeyEqual, class Alloc>
  void swap(::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &rhs) noexcept {
    lhs.swap(rhs);
  }

} // namespace Concurrent

#endif // UNORDERED_CONCURRENT_MAP_H