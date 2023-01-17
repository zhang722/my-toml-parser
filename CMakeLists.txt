cmake_minimum_required(VERSION 3.1.0)

project(mytoml)

set(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS}")

include_directories(./include)

add_library(toml SHARED
    ./src/parser.cpp
)
