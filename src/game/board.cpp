#include <iostream>
#include <limits>
#include <algorithm>

#include "board.h"
#include "platform.h"


Board::Board(Board const& b)
{
  scoreForPlayerX_ = b.scoreForPlayerX_;
  state_.resize(b.state_.size());
  for (size_t y = 0; y < b.state_.size(); ++y)
  {
    const size_t width = b.state_[y].size();
    state_[y].resize(width);
    for (unsigned x = 0; x < width; ++x)
    {
      state_[y][x] = b.state_[y][x];
    }
  }
}


void Board::print(const std::vector<Move>* movesToHighlight) const
{
  const std::string symbols[Occupation_Total] = { "O", "X" };
#ifdef USE_ANSI
  const std::string moveHighlight = "\x1b[7m";
  const std::string playerColoring[Occupation_Total] = { "\x1b[31m", "\x1b[34m" };  // ANSI color codes
  const std::string resetColoring = "\x1b[0m";
#else
  const std::string moveHighlight = "";
  const std::string playerColoring[Occupation_Total] = { "", "" };  // ANSI color codes
  const std::string resetColoring = "";
#endif

  for (int y = (int)state_.size() - 1; y >= 0; --y)   // (Top to bottom)
  {
    for (int x = 0; x < (int)state_[y].size(); ++x)
    {
      if (movesToHighlight)
      {
        // Was this location one of the moves?
        for (std::vector<Move>::const_iterator mit = movesToHighlight->begin(); mit != movesToHighlight->end(); ++mit)
        {
          if (mit->locX_ == x && mit->locY_ == y)
          {
            // Highlight it
            if (Platform::doTextColorCodesWork())
              std::cout << moveHighlight;
            break;
          }
        }
      }

      // Colored symbol
      const Occupation occ = state_[y][x];
      if (Platform::doTextColorCodesWork())
        std::cout << playerColoring[occ];
      std::cout << symbols[occ];
      if (Platform::doTextColorCodesWork())
        std::cout << resetColoring;
    }
    std::cout << std::endl;
  }
}


bool Board::loadFromText(const std::vector<std::string>& lines)
{
  size_t shortestLine = std::numeric_limits<size_t>::max();
  state_.clear();
  for (size_t lineNum = 0; lineNum < lines.size(); ++lineNum)
  {
    const std::string& line = lines[lineNum];

    // Make room for a new line
    state_.resize(state_.size() + 1);

    for (std::string::const_iterator it = line.begin(); it != line.end(); ++it)
    {
      // Skip whitespace
      if (*it == ' ' || *it == '\t')
        continue;
      const Occupation occ = charToOccupation(*it);
      if (occ == Occupation_Total)
      {
        // Character was not part of the board definition; skip the rest of the line
        break;
      }
      // Add it to the row
      state_.back().push_back(occ);
    }
    
    // Did we get any data?
    if (state_.back().empty())
      state_.resize(state_.size() - 1);
    else
      shortestLine = std::min(shortestLine, state_.back().size());
  }

  // Clip all rows to the shortest length
  for (size_t row = 0; row < state_.size(); ++row)
    state_[row].resize(shortestLine);

  // The data came in from top to bottom; we want to store it with the y origin at the bottom
  std::reverse(state_.begin(), state_.end());

  return getWidth() && getHeight();
}


Occupation Board::getLocation(int x, int y) const
{
  std::pair<int, int> point = calcWrappedCoordinates(x, y);
  return state_[point.second][point.first];
}


void Board::applyMove(const Move& move)
{
  setLocation(move.locX_, move.locY_, oppositePlayer(getLocation(move.locX_, move.locY_)));
}


void Board::setLocation(int x, int y, Occupation occ)
{
  std::pair<int, int> point = calcWrappedCoordinates(x, y);
  state_[point.second][point.first] = occ;
}


std::pair<int, int> Board::calcWrappedCoordinates(int x, int y) const
{
  int row = y, col = x;
  const int width = (int)getWidth();
  const int height = (int)getHeight();
  while (row < 0)         row += height;
  while (row >= height)   row -= height;
  while (col < 0)         col += width;
  while (col >= width)    col -= width;
  return std::make_pair(col, row);
}
