//
// Created by Benny Khoo on 31/10/2016.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "xgame.hpp"

int main(int argc, char* argv[])
{
    std::string board_file = "small2.txt";

    std::ifstream file;
    file.open(board_file.c_str());
    if (!file.good())
    {
        std::cerr << "Error loading game parameters" << std::endl;
        return 1;
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


}