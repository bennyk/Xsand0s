#pragma once

#include <ostream>

struct Move
{
    int x, y;

    Move(int locX, int locY) :
            x(locX),
            y(locY)
    {}

    bool operator==(const Move& m) const  { return x == m.x && y == m.y; }
    bool is_valid() const                 { return x >= 0 && y >= 0; }

    const static Move Invalid;
};

std::ostream& operator<<(std::ostream& os, const Move& m);

typedef char Occupation;
enum {
    Occupation_PLAYER_O = 'O',
    Occupation_PLAYER_X = 'X',
    Occupation_Invalid
};

Occupation opposite_player(Occupation occ);
Occupation char_to_occupation(char c);
char occupation_to_char(Occupation occ);
