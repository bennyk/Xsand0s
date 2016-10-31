
#include <algorithm>

#include "xboard.hpp"

class XGame {
    XBoard _board;

public:
    bool loadLines(const std::vector<std::string>& lines)
    {
        assert(lines.size() > 0);

        std::vector<std::string> board_data;

        // parse some game parameters
        for (auto &line: lines) {
            board_data.push_back(line);
        }

        assert(board_data.size() > 0);

        _board.loadLines(board_data);
        return true;
    }

    void print(int extend = 0)
    {
        _board.print(extend);
    }
};