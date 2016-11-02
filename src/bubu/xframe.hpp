
#include "two_d_array.hpp"
#include "helpers.hpp"

class XFrame {
    TwoDArray<Occupation> _state;
    TwoDArray<int> _vicinity_map;
    Occupation _occupation; // my occupation

public:
    XFrame() : _state{}, _vicinity_map{}, _occupation{Occupation_Invalid}
    {}

    XFrame(const XFrame &other)
    : _state{other._state},
      _vicinity_map{other._vicinity_map},
      _occupation{other._occupation}
    {}

    int xsize() const { return _state.xsize(); }
    int ysize() const { return _state.ysize(); }

    bool loadLines(const std::vector<std::string>& lines, Occupation my_occ) {
        _occupation = my_occ;

        std::string buffer = "";

        size_t xsize = lines[0].length();
        size_t ysize = lines.size();

        for (auto &line: lines) {
            auto found = line.find_first_not_of("XO");
            if (found != std::string::npos) {
                std::cerr << "Invalid char in board data at pos: " << line[found] << " at position: " << found << std::endl;
                exit(-1);
            }
            buffer.append(line);
        }
        _state.reset(ysize, xsize, buffer.data());
        recalculate_vicinity_map();
        return true;
    }

    void reset(int ysize, int xsize, char my_occ)
    {
        _occupation = my_occ;
        _state.reset(ysize, xsize, opposite_player(my_occ));
        _vicinity_map.reset(_state.ysize(), _state.xsize(), 0);
    }

    void recalculate_vicinity_map()
    {
        assert(_occupation != Occupation_Invalid);

        _vicinity_map.reset(_state.ysize(), _state.xsize(), 0);

        for (int j = 0; j < _state.ysize(); j++) {
            for (int i = 0; i < _state.xsize(); i++) {
                _vicinity_map.put(i, j, neighbors_count_at(i, j, _occupation));
            }
        }
    };

    int neighbors_count_at(int x, int y, Occupation occ_type)
    {
        unsigned count = 0;
        const int dist_from_center = 1;
        for (int j = -dist_from_center; j <= dist_from_center; ++j)
        {
            for (int i = -dist_from_center; i <= dist_from_center; ++i)
            {
                if (i || j) { // exclude origin from count
                    char occ = _state.at(x + i, y + j);
                    count += (occ == occ_type);
                }
            }
        }
        return count;
    }

    Occupation get(int x, int y) const
    {
        return _state.at(x, y);
    }

    void toggle(int x, int y)
    {
        assert(_occupation != Occupation_Invalid);

        Occupation new_occ = opposite_player(_state.at(x, y));
        _state.put(x, y, new_occ);

        int neighbor_offset = new_occ == _occupation ? 1 : -1;
        update_vicinity_at(x, y, neighbor_offset);
    }

    void update_vicinity_at(int x, int y, int offset)
    {
        const int dist_from_center = 1;
        for (int j = -dist_from_center; j <= dist_from_center; ++j) {
            for (int i = -dist_from_center; i <= dist_from_center; ++i) {
                if (i || j) {
                    _vicinity_map.put(x + i, y + j, _vicinity_map.at(x + i, y + j) + offset);
                }
            }
        }
    }

    void apply_predicate_in_place()
    {
        for (int y = 0; y < _vicinity_map.ysize(); y++) {
            for (int x = 0; x < _vicinity_map.xsize(); x++) {
                int count = _vicinity_map.at(x, y);
                if (count == 2 || count == 3 || count == 5 || count == 6) {
//                    std::cout << "count " << count << " not toggling at " << x << "," << y << std::endl;
                } else {
//                    std::cout << "count " << count << " toggle at " << x << "," << y << std::endl;
                    _state.put(x, y, opposite_player(_state.at(x, y)));
                }
            }
        }
    }

    XFrame apply_predicate() const
    {
        XFrame result(*this);
        for (int y = 0; y < _vicinity_map.ysize(); y++) {
            for (int x = 0; x < _vicinity_map.xsize(); x++) {
                int count = _vicinity_map.at(x, y);
                if (count == 2 || count == 3 || count == 5 || count == 6) {
//                    std::cout << "count " << count << " not toggling at " << x << "," << y << std::endl;
                } else {
//                    std::cout << "count " << count << " toggle at " << x << "," << y << std::endl;
                    result.toggle(x, y);
                }
            }
        }
        return result;
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
