#pragma once

#include <string>

typedef unsigned char Occupation;

enum { Occupation_PLAYER_O,
       Occupation_PLAYER_X,
       
       Occupation_Total };

// Returns the opponent player, if valid.  Otherwise returns empty.
Occupation oppositePlayer(Occupation occ);

// Text manipulation
Occupation charToOccupation(char c);
char occupationToChar(Occupation occ);
