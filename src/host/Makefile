#CC = /usr/intel/pkgs/gcc/4.9.3/bin/g++
CC = gcc
CCFLAGS = -Wall -Werror -std=c++0x -Isrc -pthread
DEBUG = -O0 -gdwarf-2
OPTIMIZED = -O2


SOURCES =  \
	host.cpp \
	playerProxy.cpp \
	../game/board.cpp \
	../game/game.cpp \
	../game/move.cpp \
	../game/network.cpp \
	../game/occupation.cpp \
	../game/platform.cpp \
	../game/predicate.cpp

build/host: $(SOURCES)
	$(CC) $(CCFLAGS) $(OPTIMIZED) -o build/host $(SOURCES)
