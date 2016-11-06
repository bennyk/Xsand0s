#pragma once

#include <ostream>

struct Move
{
    int x, y;

    Move(int locX, int locY) :
            x(locX),
            y(locY)
    {}
    Move() : x{-1}, y{-1} {}

    Move(const Move &other) : x(other.x), y(other.y) {}

    bool operator==(const Move& m) const  { return x == m.x && y == m.y; }
    bool operator!=(const Move& m) const  { return x != m.x || y != m.y; }
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


template <typename I>
I random_element(I begin, I end)
{
    const unsigned n = (unsigned)std::distance(begin, end);
    const unsigned divisor = ((unsigned) RAND_MAX + 1) / n;

    unsigned k;
    do { k = std::rand() / divisor; } while (k >= n);

    std::advance(begin, k);
    return begin;
}
