cmake_minimum_required(VERSION 3.6)
project(bubu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        network.cpp
        network.h
        helpers.cpp
        helpers.hpp
        xplayerInterface.cpp
        xplayerInterface.h
        )

add_executable(bubu ${SOURCE_FILES} bubu.cpp)
add_executable(mcts ${SOURCE_FILES} mcts.cpp)

