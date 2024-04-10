cmake_minimum_required(VERSION 3.0)
project(SIC-XE-ASSEMBLER-TESTS)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  assembler_test
  assembler_test.cc
)
target_link_libraries(
  assembler_test
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(assembler_test)
