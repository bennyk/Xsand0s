
#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <thread>
#include <map>
#include <math.h>
#include <random>

#include "xgame.hpp"
#include "xplayerInterface.h"

struct Node;

using frame_ptr = std::shared_ptr<XFrame>;
using const_frame_ptr = std::shared_ptr<const XFrame>;
//using node_ptr = std::shared_ptr<Node>;

struct MoveState
{
    Move move;
    const_frame_ptr state;

    MoveState() : move{Move::Invalid} {}
    MoveState(const MoveState &m) : move{m.move}, state{m.state} {}
    MoveState(Move m, const_frame_ptr s) : move{m}, state{s} {}
};

struct Node
{
    using node_ptr = std::unique_ptr<Node>;

    Node *parent;
    std::vector<node_ptr> children;
    MoveState move_state;
    int wins, plays;
    bool terminal;

    Node(const MoveState &ms) : parent{nullptr}, children{}, move_state{ms}, wins{0}, plays{0}, terminal{false} {}

    Node *append_child(const MoveState &ms)
    {
        Node *child_node = new Node(ms);
        child_node->parent = this;
        children.push_back(std::unique_ptr<Node>(child_node));

        return child_node;
    }

};

struct SimResult
{
    int score;
    SimResult(int s) : score{s} {}
//    SimResult(SimResult &other) : score{other.score} {}

    bool win() const { return score > 0; }
};

class MCTS_Agent : public XPlayerInterface
{
    int _max_frames, _xsize, _ysize;
    int _current_frame_index = 0;

public:

    virtual void playGame() {
        const XGame &g = getGame();

        _current_frame_index = g.current_frame_index();
        _xsize = g.xsize();
        _ysize = g.ysize();
        _max_frames = g.num_frames();

        while (!g.is_over()) {
            const int working_frame_index = g.current_frame_index();
            std::cout << "working on frame: " << working_frame_index << std::endl;

            std::unique_ptr<Node> root(new Node(MoveState(Move::Invalid, g.current_frame())));

            int sim_count = 0;
            Move last_move;
            while (working_frame_index == _current_frame_index)
            {
                auto picked_node = select_node(root.get());
                SimResult result = simulate(picked_node);
                back_propagate(picked_node, result.win());

                Move move = Move::Invalid;
                best_move(root.get(), move);
                if (move != last_move) {
                    makeAMove(move);
                    last_move = move;
                }

                sim_count += 1;
            }

            for (auto &child: root->children) {
                std::cout << child->move_state.move << ": (" << child->wins << "/" << child->plays << ")" << std::endl;
            }
            std::cout << sim_count << " number of simulations performed." << std::endl;
            std::cout << std::endl;

        }
    }


    virtual void nextFrame()
    {
        _current_frame_index++;
    }

    Node *select_node(Node *from_node)
    {
        Node *current_node = from_node;
        Node *result = nullptr;

        auto state = from_node->move_state.state;
        long size = state->xsize() * state->ysize();

        //TODO need tunings
//        const unsigned saturation_threshold = ceil(size * .01);
        const unsigned saturation_threshold = 300;

        int depth = 0;

        while(true) {
            bool has_legal_moves = false;
            if (current_node->move_state.state->frame_index() < _max_frames) {
                has_legal_moves = true;
            }

            if (!has_legal_moves) {
                // TODO the node selection never reach enough depth.
                assert(current_node->terminal);
                result = current_node;
                break;
            }
            else if (current_node->children.size() < saturation_threshold) {

                auto move = select_move_randomly();

                MoveState move_state(move, current_node->move_state.state);

                // TODO this is an expensive ops
                auto child_move_state = apply_move_and_generate_next_frame(move_state);
                auto child = current_node->append_child(child_move_state);

                if (child->move_state.state->frame_index() == _max_frames) {
                    child->terminal = true;
                }

                result = child;
                break;
            }
            else {
//                std::cout << "compute_best_child()" << std::endl;

                // descend to the next best child
                current_node = compute_best_child(current_node);
                depth += 1;
            }
        }

        return result;
    }

    SimResult simulate(const Node * node) {
        // trying to keep this as even number. One predicate has already been applied in the input move_state during node selection.
        const int predicate_depth = 11;
        const bool simulate_opponent = false;

        if (node->move_state.state->frame_index() >= _max_frames) {
            // TODO we would be double counting score/wins/plays while back-propagation on terminal nodes.
            // Not sure if it is desirable. To test for double counting simply assert node->score != 0.

            assert(node->terminal);
            return SimResult(node->move_state.state->calculate_score());
        }

        auto starting_frame = node->move_state.state;

        XFrame scratch(*starting_frame);
        int i = starting_frame->frame_index();
        int limit = i + predicate_depth;

        // simulate initial opponent move

        if (simulate_opponent) {
            // the first move that led to this game state has already been taken in select_node()
            // so this is to simulate a single opponent.
            auto move0 = select_move_randomly();
            scratch.toggle(move0.x, move0.y);
        }

        while (i < limit && i < _max_frames)
        {
            // simulate random moves committed by me and opponent
            if (simulate_opponent) {
                auto move1 = select_move_randomly();
                auto move2 = select_move_randomly();

                std::cout << "simulating random moves: " << move1 << " " << move2 << std::endl;
                scratch.toggle(move1.x, move1.y);
                scratch.toggle(move2.x, move2.y);
            }


            scratch.apply_predicate_in_place();

//            if (true) {
//                int score = scratch.calculate_score();
//                std::cout << i << ": " << score << std::endl;
//            }

            i++;
        }

        return SimResult(scratch.calculate_score());
    }

    Node *compute_best_child(const Node *node)
    {
        const double C = 1;

        std::map<Node *, double> values;

        for (auto &child: node->children) {
            int parent_plays = node->plays;
            assert(parent_plays > 0);

            double ucb_score = (child->wins / child->plays) + C * sqrt(2 * log(parent_plays / child->plays));
            values.insert(std::make_pair(child.get(), ucb_score));
        }

        using value_pair = std::pair<Node *, int>;

        auto result = std::max_element(values.begin(), values.end(),
                                  [](const value_pair& p1, const value_pair& p2) {
                                      return p1.second < p2.second; });
        return result->first;
    }

    void back_propagate(Node *node, int delta)
    {
        auto current_node = node;
        while (current_node->parent != nullptr)
        {
            current_node->plays += 1;
            current_node->wins += delta;
            current_node = current_node->parent;
        }

        // update root node of entire tree
        current_node->plays += 1;
        current_node->wins += delta;
    }

    void best_move(const Node *node, Move &result)
    {
        int most_plays = INT_MIN;
        int best_wins = INT_MIN;

        std::vector<Move> best_moves;

        for (auto &child: node->children) {
            if (child->plays > most_plays) {
                most_plays = child->plays;
                best_wins = child->wins;
                best_moves = {child->move_state.move};
            }
            else if (child->plays == most_plays) {
                if (child->wins > best_wins) {
                    best_wins = child->wins;
                    best_moves = {child->move_state.move};
                }
                else if (child->wins == best_wins) {
                    best_moves.push_back(child->move_state.move);
                }
            }
        }

        auto iter = random_element(best_moves.begin(), best_moves.end());
        result = *iter;
    }

    Move select_move_randomly()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> xdis(0, _xsize);
        std::uniform_int_distribution<> ydis(0, _ysize);

        return Move(xdis(gen), ydis(gen));
    }

    MoveState apply_move_and_generate_next_frame(const MoveState &move_state)
    {
        MoveState result;

        auto move = move_state.move;
        auto starting_frame = move_state.state;

        result.move = move;

        frame_ptr scratch = std::make_shared<XFrame>(*starting_frame);
        scratch->toggle(move.x, move.y);
        scratch->apply_predicate_in_place();
        scratch->set_frame_index(starting_frame->frame_index() + 1);

        result.state =  scratch;

        return result;
    }

};


int main(int argc, char* argv[])
{
    MCTS_Agent agent;
    agent.playerProgramEntryPoint(argc, argv);
    return 0;
}

