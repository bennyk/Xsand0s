#pragma once

#include <algorithm>
#include <sstream>

#include "xframe.hpp"
#include "helpers.hpp"

class XGame {
    std::unique_ptr<XFrame> _current_frame;
    int _current_frame_index = 0, _nframes = 0;

public:
    const XFrame *current_frame() { return _current_frame.get(); }
    int current_frame_index() const { return _current_frame_index; }

    int xsize() const { return _current_frame->xsize(); }
    int ysize() const { return _current_frame->ysize(); }

    int num_frames() const { return _nframes; }

    bool is_over() const { return _current_frame_index > _nframes; }

    void reset(int ysize, int xsize)
    {
        _current_frame.reset(new XFrame());
        _current_frame->reset(ysize, xsize);
    }

    bool loadLines(const std::vector<std::string>& lines)
    {
        assert(lines.size() > 0);

        _current_frame.reset(new XFrame());
        _current_frame_index = 0;

        std::vector<std::string> board_data;

        // parse some game parameters
        for (auto &line: lines) {
            std::stringstream ss(line);
            std::string param;
            ss >> param;
            //TODO test for invalid char in line stream
            if (param == "frames")
                ss >> _nframes;
            else
                board_data.push_back(line);
        }
        assert(board_data.size() > 0);

        _current_frame->loadLines(board_data);
        return true;
    }

    void apply_moves_and_generate_next_frame(const std::vector<Move>& moves)
    {
        for (auto &m: moves) {
//            if (move[m0].locX_ < 0 || moves[m0].locX_ >= (int)getBoardWidth())
            if (m.x < 0 || m.x >= xsize()) {
                std::cerr << "Invalid move in x-range: " << m << std::endl;
                continue;
            }

            if (m.y < 0 || m.y >= ysize()) {
                std::cerr << "Invalid move in y-range: " << m << std::endl;
                continue;
            }
            apply_move(m.x, m.y);
        }
        generate_next_frame();

        // TODO compute player score
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

    void print(int extend = 0) const
    {
        std::cout << "frame index: " << _current_frame_index << "/" << _nframes << std::endl;
        _current_frame->print(extend);
    }
};