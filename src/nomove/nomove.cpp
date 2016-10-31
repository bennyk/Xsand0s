#include "../game/playerInterface.h"


class NoMovePlayer : public PlayerInterface
{
    volatile bool resetSearch_;

public:
    NoMovePlayer() : PlayerInterface(), resetSearch_(false) {}
    void playGame();
    void nextFrame();
};


void NoMovePlayer::playGame()
{
    while (!getGame().isOver())
    {
    }
}


void NoMovePlayer::nextFrame()
{
    // Inform the AI that a new frame was triggered
    resetSearch_ = true;
    Game g = getGame();
    g.printBoard(99);
}


int main(int argc, char* argv[])
{
    NoMovePlayer playerInterface;
    return playerInterface.playerProgramEntryPoint(argc, argv);
}
