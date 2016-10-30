#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "move.h"
#include "occupation.h"


class Board
{
  std::vector<std::vector<Occupation> > state_;
  double scoreForPlayerX_;


public:

  Board() : scoreForPlayerX_(0)   {}
  Board(Board const& b);

  size_t getWidth() const     { if (!getHeight()) return 0; else return state_[0].size(); }
  size_t getHeight() const    { return state_.size(); }

  // Loads a board from the given text data. Returns false if there was a parse error.
  bool loadFromText(const std::vector<std::string>& lines);

  // Calculated score based on the game progression for a player
  void setScoreForPlayerX(double score)               { scoreForPlayerX_ = score; }
  double getScoreForPlayer(Occupation player) const   { return (player == Occupation_PLAYER_X ? 1.0 : -1.0) * scoreForPlayerX_; }

  // Value at the specified location (coordinates will be wrapped if necessary)
  Occupation getLocation(int x, int y) const;


  // Update a cell on the board due to a player move
  void applyMove(const Move& move);

  // Update a cell on the board (coordinates will be wrapped if necessary)
  void setLocation(int x, int y, Occupation occ);


  // Text dump
  void print(const std::vector<Move>* movesToHighlight) const;

private:

  // Convert world coordinates to local in-range coordinates
  std::pair<int, int> calcWrappedCoordinates(int x, int y) const;
};
