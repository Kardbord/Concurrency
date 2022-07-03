#ifndef UNORDERED_CONCURRENT_MAP_H
#define UNORDERED_CONCURRENT_MAP_H

#include <shared_mutex>
#include <unordered_map>

namespace Concurrent {

  // This class provides a thread-safe unordered map with most of the same functionality as
  // std::unordered_map. Functions which behave differently than their std::unordered_map
  // counterpart of the same name are documented with comments, as are functions that
  // do not exist for std::unordered_map.
  //
  // TODO: Remove footguns. Functions that return non-const iterators or pointers should be examined.
  // TODO: Provide same constructors as std::unordered_map.
  // TODO: Provide same operator= overloads as std::unordered_map.
  // TODO: Provite cbegin and cend?
  // TODO: Provite cbegin and cend for buckets?
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
    using insert_return_type   = typename internal_map_type::insert_return_type;

    // ------------------------------ Constructors ------------------------------ //
    UnorderedMap()  = default;
    ~UnorderedMap() = default;

    UnorderedMap(const UnorderedMap &)            = delete;
    UnorderedMap(UnorderedMap &&)                 = delete;
    UnorderedMap &operator=(const UnorderedMap &) = delete;
    UnorderedMap &operator=(UnorderedMap &&)      = delete;

    allocator_type get_allocator() const { return m_map.get_allocator(); }

    // ------------------------------- Iterators -------------------------------- //
    /*
    Iterators are not supported due to the footgun they present
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

    std::pair<iterator, bool> insert(const value_type &value) {
      auto lock = lock_for_writing();
      return m_map.insert(value);
    }
    std::pair<iterator, bool> insert(value_type &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(value);
    }
    template <class P>
    std::pair<iterator, bool> insert(P &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(value);
    }
    iterator insert(const_iterator hint, const value_type &value) {
      auto lock = lock_for_writing();
      return m_map.insert(hint, value);
    }
    iterator insert(const_iterator hint, value_type &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(hint, value);
    }
    template <class P>
    iterator insert(const_iterator hint, P &&value) {
      auto lock = lock_for_writing();
      return m_map.insert(hint, value);
    }
    template <class InputIt>
    void insert(InputIt first, InputIt last) {
      auto lock = lock_for_writing();
      return m_map.insert(first, last);
    }
    void insert(std::initializer_list<value_type> ilist) {
      auto lock = lock_for_writing();
      return m_map.insert(ilist);
    }
    insert_return_type insert(node_type &&nh) {
      auto lock = lock_for_writing();
      return m_map.insert(nh);
    }
    iterator insert(const_iterator hint, node_type &&nh) {
      auto lock = lock_for_writing();
      return m_map.insert(nh);
    }

    template <class M>
    std::pair<iterator, bool> insert_or_assign(const Key &k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(k, obj);
    }
    template <class M>
    std::pair<iterator, bool> insert_or_assign(Key &&k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(k, obj);
    }
    template <class M>
    iterator insert_or_assign(const_iterator hint, const Key &k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(hint, k, obj);
    }
    template <class M>
    iterator insert_or_assign(const_iterator hint, Key &&k, M &&obj) {
      auto lock = lock_for_writing();
      return m_map.insert_or_assign(hint, k, obj);
    }

    template <class... Args>
    std::pair<iterator, bool> emplace(Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.emplace(args...);
    }

    template <class... Args>
    iterator emplace_hint(const_iterator hint, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.emplace_hint(hint, args...);
    }

    template <class... Args>
    std::pair<iterator, bool> try_emplace(const Key &k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(k, args...);
    }
    template <class... Args>
    std::pair<iterator, bool> try_emplace(Key &&k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(k, args...);
    }
    template <class... Args>
    iterator try_emplace(const_iterator hint, const Key &k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(hint, k, args...);
    }
    template <class... Args>
    iterator try_emplace(const_iterator hint, Key &&k, Args &&...args) {
      auto lock = lock_for_writing();
      return m_map.try_emplace(hint, k, args...);
    }

    iterator erase(iterator pos) {
      auto lock = lock_for_writing();
      return m_map.erase(pos);
    }
    iterator erase(const_iterator pos) {
      auto lock = lock_for_writing();
      return m_map.erase(pos);
    }
    iterator erase(const_iterator first, const_iterator last) {
      auto lock = lock_for_writing();
      return m_map.erase(first, last);
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

    node_type extract(const_iterator position) {
      auto lock = lock_for_writing();
      return m_map.extract(position);
    }
    node_type extract(const Key &k) {
      auto lock = lock_for_writing();
      return m_map.extract(k);
    }

    template <class H2, class P2>
    void merge(std::unordered_map<Key, Val, H2, P2, Allocator> &source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    template <class H2, class P2>
    void merge(std::unordered_map<Key, Val, H2, P2, Allocator> &&source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    template <class H2, class P2>
    void merge(std::unordered_multimap<Key, Val, H2, P2, Allocator> &source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }
    template <class H2, class P2>
    void merge(std::unordered_multimap<Key, Val, H2, P2, Allocator> &&source) {
      auto lock = lock_for_writing();
      m_map.merge(source);
    }

    // ------------------------------ Accessors --------------------------------- //
    const Val &at(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.at(key);
    }

    // Returns a constant reference to the element mapped to
    // the provided key.
    const Val &operator[](const Key &key) const { return at(key); }
    // Returns a constant reference to the element mapped to
    // the provided key.
    const Val &operator[](Key &&key) const { return at(key); }

    size_type count(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.count(key);
    }

    const_iterator find(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.find(key);
    }

    std::pair<const_iterator, const_iterator> equal_range(const Key &key) const {
      auto lock = lock_for_reading();
      return m_map.equal_range(key);
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
      return m_map.lock_for_reading();
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

    // ----------------------------- Lock Interface ----------------------------- //

    // Returns a locked read_lock that prevents concurrent write access to
    // the underlying map.
    read_lock lock_for_reading() { return read_lock(m_mutex); }

    // Returns a locked write_lock that prevents concurrent access to the
    // underlying map.
    write_lock lock_for_writing() { return write_lock(m_mutex); }

  private:
    mutex_type m_mutex{};
    internal_map_type m_map{};
  };

} // namespace Concurrent

template <class Key, class T, class Hash, class KeyEqual, class Alloc>
bool operator==(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const std::unordered_map<Key, T, Hash, KeyEqual, Alloc> &rhs) {
  auto lhs_lock = lhs.lock_for_reading();
  auto rhs_lock = rhs.lock_for_reading();
  return lhs.data() == rhs.data();
}

template <class Key, class T, class Hash, class KeyEqual, class Alloc>
bool operator!=(const ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, const std::unordered_map<Key, T, Hash, KeyEqual, Alloc> &rhs) {
  return !(lhs == rhs);
}

// Specializes the std::swap algorithm for ::Concurrent::UnorderedMap. Swaps the contents of lhs and rhs. Calls lhs.swap(rhs).
template <class Key, class T, class Hash, class KeyEqual, class Alloc>
void swap(::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &lhs, ::Concurrent::UnorderedMap<Key, T, Hash, KeyEqual, Alloc> &rhs) noexcept {
  lhs.swap(rhs);
}

#endif // UNORDERED_CONCURRENT_MAP_H