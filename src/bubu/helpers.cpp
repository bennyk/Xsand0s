
#include "helpers.hpp"

const Move Move::Invalid(-1, -1);


std::ostream& operator<<(std::ostream& os, const Move& m)
{
    os << "move(" << m.x << "," << m.y << ")";
    return os;
}
