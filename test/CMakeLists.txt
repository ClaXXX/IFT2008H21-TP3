cmake_minimum_required(VERSION 3.5)
project(TP3)

include(GoogleTest)

include_directories(../src)
enable_testing()
add_executable(Test main.cpp)
target_link_libraries(Test gtest gtest_main pthread)

gtest_add_tests(Test "" AUTO)
