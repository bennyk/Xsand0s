
#include "two_d_array.hpp"

class XBoard {
    TwoDArray<char> _state;
    TwoDArray<int> _vicinity_map;

public:
    bool loadLines(const std::vector<std::string>& lines) {
        std::string buffer = "";

        size_t xsize = lines[0].length();
        size_t ysize = lines.size();

        for (auto &line: lines) {
            buffer.append(line);
        }
        _state.reset(ysize, xsize, buffer.data());
        recalculate_vicinity_map();
        return true;
    }

    void recalculate_vicinity_map()
    {
        _vicinity_map.reset(_state.ysize(), _state.xsize());

        for (int j = 0; j < _state.ysize(); j++) {
            for (int i = 0; i < _state.xsize(); i++) {
                _vicinity_map.put(i, j, neighbors_count_at(i, j, 'X'));
            }
        }
    };

    int neighbors_count_at(int x, int y, char occ_type)
    {
        unsigned count = 0;
        const int dist_from_center = 1;
        for (int j = -dist_from_center; j <= dist_from_center; ++j)
        {
            for (int i = -dist_from_center; i <= dist_from_center; ++i)
            {
                if (i || j) {
                    char occ = _state.at(x + i, y + j);
                    count += (occ == occ_type);
                }
            }
        }
        return count;
    }

    void print(int extend=0)
    {

        std::cout << "board: "
                  << _state.ysize() << "x" << _state.xsize();
        if (extend > 0)
            std::cout << " extend: " << extend;
        std::cout << std::endl;
        _state.print(extend);

        std::cout << "vicinity map:" << std::endl;
        _vicinity_map.print();
    }

};
