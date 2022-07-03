#include <iostream>
#include <cstdlib>
#include <UnorderedConcurrentMap.h>

int main() {
  Concurrent::UnorderedMap<std::string, std::string> a;
  Concurrent::UnorderedMap<std::string, std::string> b;
  a.swap(b);
  return EXIT_SUCCESS;
}