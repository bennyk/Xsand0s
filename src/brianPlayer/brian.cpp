/*
 * Example player that does a simple greedy search for the move that generates the best score at the end of the game.
 * It will submit moves as they are discovered and keep searching until the frame expires.
 */

#include "../game/playerInterface.h"

class SimpleSimon : public PlayerInterface
{
  volatile size_t currentFrame_;

public:
  SimpleSimon() : PlayerInterface(), currentFrame_(0)  {}
  void playGame();
  void nextFrame();

  double getEndingScoreResultingFromMove(const Game& startingGame, const Move& move) const;
};


void SimpleSimon::playGame()
{
  bool verbose = false;

  while (!getGame().isOver())
  {
    const size_t workingFrame = currentFrame_;
    Game currentGame = getGame();

    // What is the score if we do nothing?
    Move bestMove = Move::Invalid;
    double bestMoveScore = getEndingScoreResultingFromMove(currentGame, bestMove);
    std::cout << "Baseline score is " << bestMoveScore << std::endl;

    size_t numLocationsTested = 0;
    while (workingFrame == currentFrame_)
    {
      // Randomly choose a cell to try
      int x = rand() % currentGame.getBoardWidth();
      int y = rand() % currentGame.getBoardHeight();
      ++numLocationsTested;

      const Move potentialMove(x, y);
      const double potentialMoveScore = getEndingScoreResultingFromMove(currentGame, potentialMove);
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

    std::cout << "Frame " << workingFrame << ": Tested " << numLocationsTested << " cells.";
    if (bestMove.isValid())
      std::cout << " Move score was " << bestMoveScore << std::endl;
    else
      std::cout << " Did not make a move; baseline score was " << bestMoveScore << std::endl;
  }
}


void SimpleSimon::nextFrame()
{
  ++currentFrame_;
}


double SimpleSimon::getEndingScoreResultingFromMove(const Game& startingGame, const Move& move) const
{
  Game simulatedGame = startingGame;
  std::vector<Move> movesToApply;
  movesToApply.push_back(move);

  while (!simulatedGame.isOver())
  {
    simulatedGame.applyMovesAndGenerateNextFrame(movesToApply);
    movesToApply.clear();
  }

  return simulatedGame.getCurrentBoard().getScoreForPlayer(getPlayerAssignment());
}


int main(int argc, char* argv[])
{
  SimpleSimon playerInterface;
  return playerInterface.playerProgramEntryPoint(argc, argv);
}
