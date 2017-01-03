#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <map>
#include <random>
#include <unordered_set>

#include "xgame.hpp"
#include "xplayerInterface.h"
#include "xqueue.hpp"


using move_state_type = std::pair<Move, std::shared_ptr<const XFrame> >;
using queue_type = XQueue<move_state_type>;
using thread_ptr = std::shared_ptr<std::thread>;


class MainAgent : public XPlayerInterface
{
public:
    class Worker
    {
        int _num_frame;
        MainAgent *_parent;
        queue_type *_q;
        int _id;
        int _cells_simulated;
        std::mutex _mu;

    public:
        Worker(MainAgent *parent, queue_type *q, int id)
                : _parent{parent}, _q{q}, _cells_simulated{0}, _id{id}
        {
            _num_frame = parent->getGame().num_frames();
        }

        Worker(const Worker &other)
                : _num_frame{other._num_frame},
                  _parent{other._parent},
                  _q{other._q},
                  _cells_simulated{other._cells_simulated},
                  _id{other._id}
        {}

        int cells_simulated() const { return _cells_simulated; }

        void operator()()
        {
            std::cout << "slave thread " << _id << " started." << std::endl;
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
            std::cout << "slave thread " << _id << " exited." << std::endl;
        }

        void notify_next_frame()
        {
            std::unique_lock<std::mutex> locker(_mu);
            _cells_simulated = 0;
        }

        static int simulate(move_state_type &move_state, int max_frames)
        {
            auto move = move_state.first;
            auto starting_frame = move_state.second;

            XFrame scratch(*starting_frame);

            if (move.is_valid()) {
                scratch.toggle(move.x, move.y);
            }

            int i = starting_frame->frame_index();
            int limit = i + 6;
            while (i < limit && i < max_frames) {
                scratch.apply_predicate_in_place();
                i++;
            }
            return scratch.calculate_score();
        }
    };

public:
    using slave_ptr = std::shared_ptr<Worker>;

    std::mutex _mu;
    int _current_frame_index = 0;
    int _bestMoveScore;
    int _xsize, _ysize;

    XQueue<move_state_type> _main_queue;
    std::vector<slave_ptr> _slaves;
    std::vector<thread_ptr> _slave_thrs;

public:
    void playGame()
    {
        const XGame &g = getGame();
        bool verbose = false;
        _xsize = g.xsize();
        _ysize = g.ysize();

//        int cores = std::thread::hardware_concurrency();
        int cores = 4;
        std::cout << "number of available cores: " << cores << std::endl;
        for (int i = 0; i < cores; i++) {
            auto s = std::make_shared<Worker>(this, &_main_queue, i+1);
            _slaves.push_back(s);
            _slave_thrs.push_back(std::make_shared<std::thread>(std::thread([&](){
                (*s)();
            })));
        }

        while (!g.is_over())
        {
            const int working_frame_index = g.current_frame()->frame_index();

            // What is the score if we do nothing?
            Move bestMove = Move::Invalid;
            move_state_type no_move = {Move::Invalid, g.current_frame()};

            std::cout << "start working on frame " << working_frame_index << std::endl;

//            int init_score = g.current_frame()->calculate_score();
//            std::cout << "initial score " << init_score << std::endl;

            _bestMoveScore = Worker::simulate(no_move, g.num_frames());
            std::cout << "Baseline score is " << _bestMoveScore << std::endl;

            std::unordered_set<Move> moves;
            while (working_frame_index == _current_frame_index)
            {
                // Randomly choose a cell to try
                Move potential_move = select_move_randomly();
                if (moves.find(potential_move) == moves.end()) {
//                    std::cout << "attempting " << potential_move << std::endl;
                    _main_queue.add(std::make_pair(potential_move, g.current_frame()));
                    moves.insert(potential_move);
                } else {
//                    std::cout << "discarding " << potential_move << std::endl;
                }

            }

            // notify slaves thread on next frame and print some report.
            int total_cells = 0;
            for (auto &s: _slaves) {
                total_cells += s->cells_simulated();
                s->notify_next_frame();
            }
            std::cout << "Frame " << _current_frame_index << ": Tested " << total_cells << " cells." << std::endl;
            std::cout << std::endl;
        }

        std::cout << "game over." << std::endl;

        join();
    }

    void join()
    {
        for (auto &t: _slave_thrs) {
            std::cout << "joining slave thread" << std::endl;
            if (t->joinable())
                t->join();
        }
    }

    virtual void nextFrame()
    {
        ++_current_frame_index;
    }

    void notify_move(move_state_type &move_state, int score)
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

    Move select_move_randomly()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xdis(0, _xsize);
        std::uniform_int_distribution<> ydis(0, _ysize);

        return Move(xdis(gen), ydis(gen));
    }

};


int main(int argc, char* argv[])
{
    MainAgent agent;
    agent.playerProgramEntryPoint(argc, argv);
    return 0;
}

