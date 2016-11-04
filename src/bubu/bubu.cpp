//
// Created by Benny Khoo on 31/10/2016.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "xgame.hpp"
#include "xplayerInterface.h"

class TestLoad
{
public:
    bool test()
    {
        std::string board_file = "small2.txt";

        std::ifstream file;
        file.open(board_file.c_str());
        if (!file.good())
        {
            std::cerr << "Error loading game parameters" << std::endl;
            return false;
        }

        std::vector<std::string> buffer;
        while (!file.eof())
        {
            std::string line;
            getline(file, line);
            if (!line.empty())
                buffer.push_back(line);
        }
        XGame game;
        game.loadLines(buffer);
        game.print();
        game.print(1);

        return true;
    }
};

class TestToggle
{
public:
    bool test()
    {
        XGame game;

        game.reset(7, 7);
        game.print();

        int startx=2, starty=2;
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 3; i++) {
                game.apply_move(startx + i, starty + j);
            }
        }
        game.print();
        return true;
    }

};

class TestPredicate
{
public:
    bool test()
    {
        std::string board_file = "small2.txt";

        std::ifstream file;
        file.open(board_file.c_str());
        if (!file.good()) {
            std::cerr << "Error loading game parameters" << std::endl;
            return false;
        }

        std::vector<std::string> buffer;
        while (!file.eof())
        {
            std::string line;
            getline(file, line);
            if (!line.empty())
                buffer.push_back(line);
        }

        XGame game;
        game.set_player_assignment(Occupation_PLAYER_X);
        game.loadLines(buffer);
        game.print();
        std::cout << std::endl;

        for (int i = 0; i < game.num_frames(); i++) {
            game.generate_next_frame();
            game.print();
            std::cout << std::endl;
        }

        return true;
    }
};

class RandomPlayer : public XPlayerInterface
{
    bool resetSearch_;

public:
    virtual void playGame()
    {
        // Make the randomization different for each player
        srand((unsigned)getGame().player_assignment());

        while (!getGame().is_over())
        {
            std::cout << "starting to search" << std::endl;
            getGame().print();

            resetSearch_ = false;
            bool moveSent = false;

            // Find an opponent's cell and grab it.  We randomly offset the coordinates each frame so we're not stuck in one corner of the board
            const unsigned offsetX = rand() % (unsigned) getGame().xsize();
            const unsigned offsetY = rand() % (unsigned) getGame().ysize();
            for (size_t y = 0; y < getGame().ysize() && !moveSent; ++y) {
                // Occasional check to see if we ran out of time
                if (resetSearch_)
                    break;

                // Search the row
                for (size_t x = 0; x < getGame().xsize() && !moveSent; ++x) {
                    const unsigned coordX = (x + offsetX) % (unsigned) getGame().xsize();
                    const unsigned coordY = (y + offsetY) % (unsigned) getGame().ysize();
                    const Occupation current_occ = getGame().current_frame()->get(coordX, coordY);
                    if (opposite_player(current_occ) == getGame().player_assignment()) {
                        // This is our move
                        Move move(coordX, coordY);
                        makeAMove(move);
                        moveSent = true;

                        XFrame scratch(*getGame().current_frame());

                        // Calculate the projected result by simulating the rest of the game as if no other moves are made
                        scratch.toggle(move.x, move.y);
                        scratch.apply_predicate_in_place();

                        int score = scratch.calculate_score();
                        std::cout << "Frame " << getGame().current_frame_index() << " move is " << move << " for a projected score of " << score << std::endl;
                        scratch.print();
                    }
                }
            }

            // Wait for the next frame
            if (!getGame().is_over()) {
                std::cout << "waiting for next frame" << std::endl;
                while (!resetSearch_) {}
            }
        }

        std::cout << "game over." << std::endl;
    }

    virtual void nextFrame()
    {
        // Inform the AI that a new frame was triggered
        std::cout << "called next frame" << std::endl;
        resetSearch_ = true;
    }
};


class OneShotPlayer : public XPlayerInterface
{
    int _current_frame_index = 0;

public:
    void playGame()
    {
        bool verbose = false;
        const int xsize = getGame().xsize();
        const int ysize = getGame().ysize();

        const XGame &g = getGame();

        while (!g.is_over())
        {
            const int working_frame_index = g.current_frame_index();

            // What is the score if we do nothing?
            Move bestMove = Move::Invalid;
            int bestMoveScore = getEndingScoreResultingFromMove(getGame().current_frame(), bestMove);
            std::cout << "Baseline score is " << bestMoveScore << std::endl;

            size_t numLocationsTested = 0;
            while (working_frame_index == _current_frame_index)
            {
                // Randomly choose a cell to try
                int x = rand() % xsize;
                int y = rand() % ysize;
                ++numLocationsTested;

                const Move potentialMove(x, y);
                int potentialMoveScore = getEndingScoreResultingFromMove(g.current_frame(), potentialMove);
                if (potentialMoveScore > bestMoveScore)
                {
                    // This is the best move we have seen so far; apply it
                    bestMove = potentialMove;
                    bestMoveScore = potentialMoveScore;
                    makeAMove(potentialMove);
                    std::cout << "Move (" << x << ", " << y << ") gives a score of " << bestMoveScore << std::endl;
                }
                else if (verbose)
                {
                    std::cout << "Rejected move (" << x << ", " << y << ") which gives a score of " << potentialMoveScore << std::endl;
                }
            }

            std::cout << "Frame " << working_frame_index << ": Tested " << numLocationsTested << " cells.";
            if (bestMove.is_valid())
                std::cout << " Move score was " << bestMoveScore << std::endl;
            else
                std::cout << " Did not make a move; baseline score was " << bestMoveScore << std::endl;

        }

    }

    int getEndingScoreResultingFromMove(const XFrame *starting_frame, const Move& move) const
    {
        const XGame &g = getGame();
        int num_frame = g.num_frames();

        XFrame scratch(*starting_frame);
        scratch.toggle(move.x, move.y);

        int i = _current_frame_index;
        int limit = _current_frame_index + 12;
        while (i < limit && i < num_frame)
        {
            scratch.apply_predicate_in_place();
            i++;
        }
        return scratch.calculate_score();
    }

    virtual void nextFrame()
    {
        ++_current_frame_index;
    }
};

class TestPlayer
{
public:
    bool test(int argc, char *argv[])
    {
//        RandomPlayer playerInterface;
        OneShotPlayer playerInterface;

        playerInterface.playerProgramEntryPoint(argc, argv);
        return true;
    }
};

int main(int argc, char* argv[])
{
//    TestLoad t;
//    t.test();

//    TestToggle t;
//    t.test();

//    TestPredicate t;
//    t.test();

    TestPlayer t;
    t.test(argc, argv);

    return 0;
}