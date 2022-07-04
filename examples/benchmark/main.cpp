#include <UnorderedConcurrentMap.h>
#include <cstdlib>
#include <iostream>

int main() {
  Concurrent::UnorderedMap<std::string, std::string> myMap{
      {"foo", "bar"},
      {"baz", "baz"},
      {"lorem", "ipsum"},
  };

  std::cout << "Contents:";
  for (auto const &[key, val] : myMap.data()) {
    std::cout << " [" << key << "]=" << val;
  }
  std::cout << "\n";

  return EXIT_SUCCESS;
}