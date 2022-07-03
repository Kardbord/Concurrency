cmake_minimum_required(VERSION 3.16.3)
project(ConcurrentMap)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

#set(CMAKE_VERBOSE_MAKEFILE ON)
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(
    -Wall -Wextra -Wpedantic -Werror
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-ggdb3>
  )
endif()

include_directories(include src)
set("${CMAKE_PROJECT_NAME}_SRC"
)

set("BENCHMARK_SRC"
  examples/benchmark/main.cpp
)

add_executable(${CMAKE_PROJECT_NAME}_benchmark ${${CMAKE_PROJECT_NAME}_SRC} ${BENCHMARK_SRC})
