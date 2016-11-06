#pragma once

#include "board.h"
#include "move.h"
#include "predicate.h"


class Game
{
  size_t numFrames_;
  double moveTimeLimitInSeconds_;
  size_t numMovesPerFramePerPlayer_;

  Predicate* predicate_;

  std::vector<Board> boards_;
  std::vector<std::vector< Move > > moves_;


public:
  Game();
  ~Game();

  // Copies the entire state of the game for speculative simulations
  Game(Game const& g);

  bool loadFile(const std::vector<std::string>& lines);

  // Game parameters
  size_t getNumFrames() const                   { return numFrames_; }
  double getMoveTimeLimitInSeconds() const      { return moveTimeLimitInSeconds_; }
  size_t getNumMovesPerFramePerPlayer() const   { return numMovesPerFramePerPlayer_; }

  const Predicate& getPredicate() const         { return *predicate_; }

  size_t getBoardWidth() const                  { if (boards_.empty()) return 0; return boards_[0].getWidth(); }
  size_t getBoardHeight() const                 { if (boards_.empty()) return 0; return boards_[0].getHeight(); }

  const std::vector<Board>& getBoards() const   { return boards_; }
  const Board& getCurrentBoard() const          { return boards_.back(); }

  bool isOver() const                           { return boards_.size() > numFrames_; }

  // Get the moves applied to the specified frame
  const std::vector<Move>& getMovesApplied(size_t frameNum) const             { return moves_[frameNum]; }


  // Update
  void applyMovesAndGenerateNextFrame(const std::vector<Move>& moves);

  // Value at the specified location (wrapped coordinates allowed)
  // for the board number reverse-indexed from the latest one.
  Occupation getLocation(int x, int y, unsigned framesFromEnd) const;


  void printBoard(size_t frameNum) const;
  void printIntermediateBoard(size_t frameNum) const;
};
