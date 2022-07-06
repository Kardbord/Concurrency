# ------------------------------ Setup ------------------------------- #
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
include_directories(include)

# -------------------------- Build Examples -------------------------- #
set("EXAMPLE_SRC"
  examples/main.cpp
)
add_executable(${CMAKE_PROJECT_NAME}_example ${EXAMPLE_SRC})

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
