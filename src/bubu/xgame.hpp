
#include <algorithm>

#include "xframe.hpp"

class XGame {
    std::unique_ptr<XFrame> _current_frame;
    int _current_frame_index = 0;

public:
    const XFrame *current_frame() { return _current_frame.get(); }

    void reset(int ysize, int xsize)
    {
        _current_frame.reset(new XFrame());
        _current_frame->reset(ysize, xsize);
    }

    bool loadLines(const std::vector<std::string>& lines)
    {
        assert(lines.size() > 0);

        _current_frame.reset(new XFrame());

        std::vector<std::string> board_data;

        // parse some game parameters
        for (auto &line: lines) {
            board_data.push_back(line);
        }
        assert(board_data.size() > 0);

        _current_frame->loadLines(board_data);
        return true;
    }

    void apply_move(int x, int y)
    {
        _current_frame->toggle(x, y);
    }

    void generate_next_frame()
    {
        auto new_frame = _current_frame->apply_predicate();
        _current_frame.reset(new XFrame(new_frame));
        _current_frame_index += 1;
    }

    void print(int extend = 0)
    {
        std::cout << "frame index: " << _current_frame_index << std::endl;
        _current_frame->print(extend);
    }
};