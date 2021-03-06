// Standard main.cpp program handler for a player
#pragma once

#include <queue>

#include "helpers.hpp"
#include "network.h"
#include "xgame.hpp"


class XPlayerInterface
{
    // Current game state
    XGame game_;

    // Communications
    int socketHandle_;
    SocketIO* io_;
    volatile bool playerIsActive_;

    // Output
    bool verbose_;


protected:

    // State info:

    // Current game state
    // *** Note that the game object may be updated asynchronously when a frame is synchronized by the host! ***
    const XGame& getGame() const { return game_; }


    // Commands:

    // Send a move to the server.  This can be done multiple times per frame; the host will use the last one received.
    void makeAMove(Move const& move);


public:

    XPlayerInterface();
    virtual ~XPlayerInterface();


    // Program flow:

    // Call this function from main().  It will not return until the game ends or there was an error.
    // Returns standard program error code (0 == success)
    int playerProgramEntryPoint(int argc, char* argv[]);

    // Implement this function with your player logic.  It should return when the game is complete.
    virtual void playGame() = 0;


    // Event notifications:

    // Called after the host has applied the players' moves and updated the frame
    virtual void nextFrame()  {}


private:

    // Handle asynchronous communication with the host
    static void hostLiaisonThreadEntryPoint(XPlayerInterface* playerInterface)   { playerInterface->hostLiaison(); }
    void hostLiaison();
};
