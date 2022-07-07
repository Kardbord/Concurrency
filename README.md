[![Build and Test](https://github.com/TannerKvarfordt/Concurrency/actions/workflows/cmake.yml/badge.svg)](https://github.com/TannerKvarfordt/Concurrency/actions/workflows/cmake.yml)

# Concurrency

This project provides thread-safe wrappers around C++ standard library containers. Most standard library functionality is preserved, though
some exceptions have to be made to remove footguns in the context of concurrent access. Notably, iterator access is not supported for most
wrappers.

## [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)

The following wrappers around [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)
are provided.

### [`Concurrent::UnorderedMap`](include/UnorderedConcurrentMap.h)

[`Concurrent::UnorderedMap`](include/UnorderedConcurrentMap.h) is a simple wrapper around
[`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map) which
allows thread-safe access via an internal [`std::shared_mutex`](https://en.cppreference.com/w/cpp/thread/shared_mutex).

### [`Concurrent::ShardedUnorderedMap`](include/ShardedUnorderedConcurrentMap.h)

[`Concurrent::ShardedUnorderedMap`](include/ShardedUnorderedConcurrentMap.h) provides the same interfaces
as [`Concurrent::UnorderedMap`](include/UnorderedConcurrentMap.h), but employs sharding in an effort to improve
write-access performance. By splitting the underlying data into multiple [`Concurrent::UnorderedMap`](include/UnorderedConcurrentMap.h)s, multiple
threads may obtain write access at once, provided the respective keys they are accessing are stored in different
shards.
