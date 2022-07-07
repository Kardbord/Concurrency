# ------------------------------ Setup ------------------------------- #
cmake_minimum_required(VERSION 3.16.3)
project(Concurrency)
set(CMAKE_CXX_STANDARD 17)
option(CodeCoverage "CodeCoverage" OFF)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/modules/)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

#set(CMAKE_VERBOSE_MAKEFILE ON)
if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(
    -Wall -Wextra -Wpedantic -Werror
    $<$<CONFIG:RELEASE>:-Ofast>
    $<$<CONFIG:DEBUG>:-O0>
    $<$<CONFIG:DEBUG>:-ggdb3>
    $<$<CONFIG:DEBUG>:-fprofile-arcs>
    $<$<CONFIG:DEBUG>:-ftest-coverage>
  )
  add_link_options(
    $<$<CONFIG:DEBUG>:-lgcov>
    $<$<CONFIG:DEBUG>:--coverage>
  )
endif()
include_directories(include)

# -------------------------- Build Examples -------------------------- #
set("SIMPLE_EXAMPLE_SRC"
  examples/simple.cpp
)
add_executable(${CMAKE_PROJECT_NAME}_simple_example ${SIMPLE_EXAMPLE_SRC})

set("BENCHMARK_SRC"
  examples/benchmark.cpp
)
add_executable(${CMAKE_PROJECT_NAME}_benchmark ${BENCHMARK_SRC})

# --------------------------- Build Tests ---------------------------- #
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
IF (WIN32)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
ENDIF()
FetchContent_MakeAvailable(googletest)
set("TEST_SRC"
  tests/UnorderedConcurrentMapTests.cpp
)
enable_testing()
add_executable(${CMAKE_PROJECT_NAME}_test ${TEST_SRC})
target_link_libraries(${CMAKE_PROJECT_NAME}_test gtest_main)
include(GoogleTest)
gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)

# -------------------------- Code Coverage --------------------------- #
if (CMAKE_BUILD_TYPE MATCHES Debug)
  if (CodeCoverage MATCHES ON)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage ${CMAKE_PROJECT_NAME}_test coverage --gtest_output=xml:coverage.junit.xml)
  endif()
endif()