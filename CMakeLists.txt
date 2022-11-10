cmake_minimum_required(VERSION 3.1.0)

project(mytoml)

# set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

include_directories(./include)

# add_executable(toml ./src/main.cpp)


# test

add_executable(test_next_block ./test/testNextBlock.cpp)
add_executable(test_parse_string ./test/testParseString.cpp)
add_executable(test_parse_number ./test/testParseNumber.cpp)