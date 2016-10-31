//
// Created by Benny Khoo on 31/10/2016.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "xgame.hpp"

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
        XBoard board = game.board();
        board.reset(7, 7);

        board.print();

        int startx=2, starty=2;
        for (int j = 0; j < 3; j++) {
            for (int i = 0; i < 3; i++) {
                board.toggle(startx + i, starty + j);
            }
        }
        board.print();
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
        game.loadLines(buffer);
        game.print();

        game.apply_predicate();
//        game.print();

        return true;
    }
};

int main(int argc, char* argv[])
{
//    TestLoad t;
//    t.test();

//    TestToggle t;
//    t.test();

    TestPredicate t;
    t.test();

    return 0;
}