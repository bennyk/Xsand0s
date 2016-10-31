cmake_minimum_required(VERSION 3.6)
project(bubu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        ../bubu/bubu.cpp)

add_executable(bubu ${SOURCE_FILES})
