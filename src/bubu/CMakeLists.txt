cmake_minimum_required(VERSION 3.6)
project(bubu)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

set(SOURCE_FILES
        network.cpp
        network.h
        helpers.cpp
        helpers.hpp
        xplayerInterface.cpp
        xplayerInterface.h
        )

# testing purposes
add_executable(bubu ${SOURCE_FILES} bubu.cpp)
add_executable(bubu2 ${SOURCE_FILES} bubu2.cpp)
#add_executable(mcts ${SOURCE_FILES} mcts.cpp)

add_executable(bubu-player ${SOURCE_FILES} bubu-player.cpp)
