cmake_minimum_required(VERSION 3.6)
project(host)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    ../game/board.cpp
    ../game/board.h
    ../game/game.cpp
    ../game/game.h
    ../game/move.cpp
    ../game/move.h
    ../game/network.cpp
    ../game/network.h
    ../game/occupation.cpp
    ../game/occupation.h
    ../game/platform.cpp
    ../game/platform.h
    ../game/playerInterface.cpp
    ../game/playerInterface.h
    ../game/predicate.cpp
    ../game/predicate.h
    host.cpp
    playerProxy.cpp
    playerProxy.h
        )

add_executable(host ${SOURCE_FILES})