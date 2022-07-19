# ------------------------------ Setup ------------------------------- #
cmake_minimum_required(VERSION 3.16.3)
project(concurrency VERSION 0.0.1
  DESCRIPTION "Sharded, thread-safe implementations of STL containers."
  HOMEPAGE_URL "https://github.com/TannerKvarfordt/Concurrency")

set(CMAKE_CXX_STANDARD 17)
option(CodeCoverage "CodeCoverage" OFF)
option(INSTALL "Set up to install the library headers on the system. The target path for installation can be set by setting the CMAKE_INSTALL_PREFIX variable." OFF)
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

# ----------------------------- Library ------------------------------ #
if(INSTALL MATCHES ON)
  include(GNUInstallDirs)
  add_library(${CMAKE_PROJECT_NAME} INTERFACE)
  target_include_directories(${CMAKE_PROJECT_NAME} INTERFACE $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
  target_compile_features(${CMAKE_PROJECT_NAME} INTERFACE cxx_std_17)

  target_sources(${CMAKE_PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/concurrency/UnorderedMap.hpp>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/concurrency/ShardedUnorderedMap.hpp>
    $<INSTALL_INTERFACE:include/concurrency/UnorderedMap.hpp>
    $<INSTALL_INTERFACE:include/concurrency/ShardedUnorderedMap.hpp>)

  install(TARGETS ${CMAKE_PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file("concurrencyConfigVersion.cmake" VERSION ${PROJECT_VERSION} COMPATIBILITY SameMajorVersion)
  configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/cmake/concurrencyConfig.cmake.in"
    "${PROJECT_BINARY_DIR}/concurrencyConfig.cmake"
    INSTALL_DESTINATION
    ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(EXPORT ${PROJECT_NAME}_Targets
    FILE ${PROJECT_NAME}Targets.cmake
    NAMESPACE ${PROJECT_NAME}::
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/${PROJECT_NAME}/cmake)

  install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME} DESTINATION include)
else()
  include_directories(include)
  # -------------------------- Build Examples -------------------------- #
  set("SIMPLE_EXAMPLE_SRC"
    examples/simple/main.cpp
    )
  add_executable(${CMAKE_PROJECT_NAME}_simple_example ${SIMPLE_EXAMPLE_SRC})

  set("MAP_BENCHMARK_SRC"
    examples/map_benchmark/main.cpp
    examples/map_benchmark/Benchmark.cpp
    )
  find_package(Threads REQUIRED)
  add_executable(${CMAKE_PROJECT_NAME}_map_benchmark ${MAP_BENCHMARK_SRC})
  target_include_directories(${CMAKE_PROJECT_NAME}_map_benchmark PRIVATE benchmark examples/map_benchmark)
  target_link_libraries(${CMAKE_PROJECT_NAME}_map_benchmark PRIVATE Threads::Threads)

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
  target_link_libraries(${CMAKE_PROJECT_NAME}_test PRIVATE gtest_main)
  include(GoogleTest)
  gtest_discover_tests(${CMAKE_PROJECT_NAME}_test)

  # -------------------------- Code Coverage --------------------------- #
  if (CMAKE_BUILD_TYPE MATCHES Debug)
    if (CodeCoverage MATCHES ON)
      include(CodeCoverage)
      setup_target_for_coverage(${PROJECT_NAME}_coverage ${CMAKE_PROJECT_NAME}_test coverage --gtest_output=xml:coverage.junit.xml)
    endif()
  endif()
endif()

