
#include <algorithm>

#include "two_d_array.hpp"

class XGame {
    std::unique_ptr<TwoDArray<char>> _board;

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

        size_t xsize = board_data[0].length();
        size_t ysize = board_data.size();

        std::cout << "board size: " << ysize << "x" << xsize << std::endl;

        std::string buffer = "";

        for (auto &line: lines) {
            buffer.append(line);
        }

        _board.reset(new TwoDArray<char>(ysize, xsize, buffer.data()));

        return true;
    }

    void print(int extend = 0)
    {
        std::cout << "board: "
                  << _board->ysize() << "x" << _board->xsize();

        if (extend > 0)
            std::cout << " extend: " << extend;

        std::cout << std::endl;

        _board->print(extend);
    }
};