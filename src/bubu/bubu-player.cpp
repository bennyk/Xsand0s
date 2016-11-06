#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <map>
#include <random>

#include "xgame.hpp"
#include "xplayerInterface.h"
#include "xqueue.hpp"


class MainAgent : public XPlayerInterface
{
public:
    using MoveStateType = std::pair<Move, std::shared_ptr<const XFrame> >;
    using QueueType = XQueue<MoveStateType>;
    std::mutex _mu;

    class Worker
    {
        int _num_frame;
        MainAgent *_parent;
        MainAgent::QueueType *_q;
        int _cells_simulated;
        std::mutex _mu;

    public:
        Worker(MainAgent *parent, MainAgent::QueueType *q)
                : _parent{parent}, _q{q}, _cells_simulated{0}
        {
            _num_frame = parent->getGame().num_frames();
        }

        Worker(const Worker &other)
                : _num_frame{other._num_frame},
                  _parent{other._parent},
                  _q{other._q},
                  _cells_simulated{other._cells_simulated}
        {}

        int cells_simulated() const { return _cells_simulated; }

        void operator()()
        {
            std::cout << "slave thread started" << std::endl;
            const XGame &g = _parent->getGame();

            while (!g.is_over())
            {
                auto move_state = _q->remove();
                if (move_state.second->frame_index() == g.current_frame()->frame_index()) {
                    std::unique_lock<std::mutex> locker(_mu);

                    int score = Worker::simulate(move_state, _num_frame);
                    _parent->notify_move(move_state, score);
                    _cells_simulated++;
                } else {
//                    std::cout << "discarded frame " << move_state.second->frame_index() << std::endl;
                }
            }
            std::cout << "slave thread exit" << std::endl;
        }

        void notify_next_frame()
        {
            std::unique_lock<std::mutex> locker(_mu);
            _cells_simulated = 0;
        }

        static int simulate(MainAgent::MoveStateType &move_state, int max_frames)
        {
            auto move = move_state.first;
            auto starting_frame = move_state.second;

            XFrame scratch(*starting_frame);

            if (move.is_valid()) {
                scratch.toggle(move.x, move.y);
            }

            int i = starting_frame->frame_index();
            int limit = i + 12;
            while (i < limit && i < max_frames) {
                scratch.apply_predicate_in_place();
                i++;
            }
            return scratch.calculate_score();
        }
    };

private:
    int _current_frame_index = 0;
    int _bestMoveScore;

public:
    void playGame()
    {
        using slave_ptr = std::shared_ptr<Worker>;
        using thread_ptr = std::shared_ptr<std::thread>;

        const XGame &g = getGame();
        bool verbose = false;
        const int xsize = g.xsize();
        const int ysize = g.ysize();

        XQueue<MoveStateType> q;
        std::vector<slave_ptr> slaves;
        std::vector<thread_ptr> slave_thrs;
        for (int i = 0; i < 4; i++) {
            auto s = std::make_shared<Worker>(this, &q);
            slaves.push_back(s);
            slave_thrs.push_back(std::make_shared<std::thread>(std::thread([&](){
                (*s)();
            })));
        }


        while (!g.is_over())
        {
            const int working_frame_index = g.current_frame()->frame_index();

            // What is the score if we do nothing?
            Move bestMove = Move::Invalid;
            MoveStateType no_move = {Move::Invalid, g.current_frame()};

            std::cout << "start working on frame " << working_frame_index << std::endl;

//            int init_score = g.current_frame()->calculate_score();
//            std::cout << "initial score " << init_score << std::endl;

            _bestMoveScore = Worker::simulate(no_move, g.num_frames());
            std::cout << "Baseline score is " << _bestMoveScore << std::endl;

            while (working_frame_index == _current_frame_index)
            {
                // Randomly choose a cell to try
                int x = rand() % xsize;
                int y = rand() % ysize;

                const Move potentialMove(x, y);
                q.add(std::make_pair(potentialMove, g.current_frame()));
            }

            int total_cells = 0;
            for (auto &s: slaves) {
                total_cells += s->cells_simulated();
                s->notify_next_frame();
            }
            std::cout << "Frame " << working_frame_index << ": Tested " << total_cells << " cells." << std::endl;
        }

        std::cout << "game over." << std::endl;

        for (auto &t: slave_thrs) {
            std::cout << "joining slave thread" << std::endl;
            t->join();
        }

    }

    virtual void nextFrame()
    {
        ++_current_frame_index;
    }

    void notify_move(MoveStateType &move_state, int score)
    {
        std::unique_lock<std::mutex> locker(_mu);

        if (move_state.first.is_valid() && score > _bestMoveScore) {
            // This is the best move we have seen so far; apply it
            _bestMoveScore = score;
            makeAMove(move_state.first);
            std::cout << "Move " << move_state.first << " gives a score of " << _bestMoveScore << std::endl;
        }
//        else {
//            std::cout << "Rejected move (" << x << ", " << y << ") which gives a score of " << potentialMoveScore << std::endl;
//        }
    }
};


int main(int argc, char* argv[])
{
    MainAgent agent;
    agent.playerProgramEntryPoint(argc, argv);
    return 0;
}

