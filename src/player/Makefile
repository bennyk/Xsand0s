CC = /usr/intel/pkgs/gcc/4.9.3/bin/g++
CCFLAGS = -Wall -Werror -std=c++0x -Isrc -pthread
DEBUG = -O0 -gdwarf-2
OPTIMIZED = -O2


SOURCES =  \
	player.cpp \
	../game/board.cpp \
	../game/game.cpp \
	../game/move.cpp \
	../game/network.cpp \
	../game/occupation.cpp \
	../game/platform.cpp \
	../game/playerInterface.cpp \
	../game/predicate.cpp

build/player: $(SOURCES)
	$(CC) $(CCFLAGS) $(OPTIMIZED) -o build/player $(SOURCES)
