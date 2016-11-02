
#include "helpers.hpp"

const Move Move::Invalid(-1, -1);


std::ostream& operator<<(std::ostream& os, const Move& m)
{
    os << "move(" << m.x << "," << m.y << ")";
    return os;
}

Occupation opposite_player(Occupation occ)
{
    switch (occ)
    {
        case Occupation_PLAYER_X:
            return Occupation_PLAYER_O;
        case Occupation_PLAYER_O:
            return Occupation_PLAYER_X;
        default:
            return Occupation_Invalid;
    }
}

Occupation char_to_occupation(char c)
{
    switch (c)
    {
        case 'X':
        case 'x':
            return Occupation_PLAYER_X;
        case 'O':
        case 'o':
            return Occupation_PLAYER_O;
        default:
            return Occupation_Invalid;
    }
}


char occupation_to_char(Occupation occ)
{
    switch (occ)
    {
        case Occupation_PLAYER_X:
            return 'X';
        case Occupation_PLAYER_O:
            return 'O';
        default:
            return ' ';
    }
}