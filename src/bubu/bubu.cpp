//
// Created by Benny Khoo on 31/10/2016.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>

#include "two_d_array.hpp"


bool loadLines(const std::vector<std::string>& lines)
{
    assert(lines.size() > 0);

    std::vector<std::string> board_data;
    for (auto &line: lines) {
        board_data.push_back(line);
    }

    assert(board_data.size() > 0);

    size_t xsize = board_data[0].length();
    size_t ysize = board_data.size();

    std::cout << "board size: " << ysize << "x" << xsize << std::endl;

    std::string buffer = "";

    for (auto &line: lines) {
        buffer.append(line);
    }
//    std::cout << "buffer: " << buffer << std::endl;

    TwoDArray<char> board(ysize, xsize, buffer.data());
    int row = -1;
//    int row = 10;
//    std::cout << "board at: " << board.at(-2, row) << board.at(-1, row) << std::endl;
    board.print();
    std::cout << "board with edge" << std::endl;
    board.print(true);

    return true;
}

int main(int argc, char* argv[])
{
    std::string board_file = "small.txt";

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
    loadLines(buffer);


}