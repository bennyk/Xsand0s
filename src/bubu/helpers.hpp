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
