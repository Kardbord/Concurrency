# Concurrency

[![Build and Test](https://github.com/TannerKvarfordt/Concurrency/actions/workflows/cmake.yml/badge.svg)](https://github.com/TannerKvarfordt/Concurrency/actions/workflows/cmake.yml)

This project provides thread-safe wrappers around C++ standard library containers. Most standard library functionality is preserved, though
some exceptions have to be made to remove footguns in the context of concurrent access. Notably, iterator access is not supported for most
wrappers.

## Installation

**NOTE** - This library requires C++17.

This is a header-only library. There are several ways to make use of the provided headers.

- [`./tools/install.sh`](./tools/install.sh). Invoke this script, optionally providing an installation directory with the `-p` option.
- Alternatively, simply copy the contents of `include/` to your system.

Either way, you'll have to make sure that the location of the headers is known your build environment. At that point, you
can include the headers as you would any others.

## Container Wrappers

### [`std::unordered_map`](https://en.cppreference.com/w/cpp/container/unordered_map)

[`::concurrency::UnorderedMap`](include/concurrency/UnorderedMap.hpp) is a simple wrapper around `std::unordered_map` which
allows thread-safe access via an internal [`std::shared_mutex`](https://en.cppreference.com/w/cpp/thread/shared_mutex).

[`::concurrency::ShardedUnorderedMap`](include/concurrency/ShardedUnorderedMap.hpp) provides the same interfaces as `::concurrency::UnorderedMap`, but employs sharding in an effort to improve
write-access performance. By splitting the underlying data into multiple `::concurrency::UnorderedMap`s, multiple
threads may obtain write access at once, provided the respective keys they are accessing are stored in different
shards. See the [map_benchmark example](examples/map_benchmark/) for performance metrics.
