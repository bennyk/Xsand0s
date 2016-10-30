#include "../game/playerInterface.h"


class ExamplePlayer : public PlayerInterface
{
  volatile bool resetSearch_;

public:
  ExamplePlayer() : PlayerInterface(), resetSearch_(false) {}
  void playGame();
  void nextFrame();
};


void ExamplePlayer::playGame()
{
  // Make the randomization different for each player
  srand(getPlayerAssignment());

  while (!getGame().isOver())
  {
    resetSearch_ = false;
    bool moveSent = false;

    // Find an opponent's cell and grab it.  We randomly offset the coordinates each frame so we're not stuck in one corner of the board
    const unsigned offsetX = rand() % (unsigned)getGame().getBoardWidth();
    const unsigned offsetY = rand() % (unsigned)getGame().getBoardHeight();
    for (size_t y = 0; y < getGame().getBoardHeight() && !moveSent; ++y)
    {
      // Occasional check to see if we ran out of time
      if (resetSearch_)
        break;

      // Search the row
      for (size_t x = 0; x < getGame().getBoardWidth() && !moveSent; ++x)
      {
        const unsigned coordX = (x + offsetX) % (unsigned)getGame().getBoardWidth();
        const unsigned coordY = (y + offsetY) % (unsigned)getGame().getBoardHeight();
        const Occupation currentOccupation = getGame().getCurrentBoard().getLocation(coordX, coordY);
        if (oppositePlayer(currentOccupation) == getPlayerAssignment())
        {
          // This is our move
          Move move(coordX, coordY);
          makeAMove(move);
          moveSent = true;

          // Calculate the projected result by simulating the rest of the game as if no other moves are made
          Game g = getGame();
          const size_t currentFrame = g.getBoards().size() - 1;

          std::vector<Move> moves;
          moves.push_back(move);
          g.applyMovesAndGenerateNextFrame(moves);
          moves.clear();
          while (!g.isOver())
          {
            g.applyMovesAndGenerateNextFrame(moves);
          }

          const double score = g.getCurrentBoard().getScoreForPlayer(getPlayerAssignment());
          std::cout << "Frame " << currentFrame << " move is (" << coordX << ", " << coordY << ") for a projected score of " << score << std::endl;
        }
      }
    }

    // Wait for the next frame
    while (!resetSearch_)
    {}
  }
}


void ExamplePlayer::nextFrame()
{
  // Inform the AI that a new frame was triggered
  resetSearch_ = true;
}


int main(int argc, char* argv[])
{
  ExamplePlayer playerInterface;
  return playerInterface.playerProgramEntryPoint(argc, argv);
}
