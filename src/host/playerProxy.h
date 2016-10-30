#pragma once

#include "../game/game.h"
#include "../game/network.h"


// Interface to player AIs
class PlayerProxy
{
protected:

  Game& game_;
  Occupation player_;
  std::string playerName_;

public:

  PlayerProxy(Game& game) :
    game_(game),
    player_(Occupation_Total)
  {}

  virtual ~PlayerProxy()  {}

  virtual void setPlayer(Occupation player)   { player_ = player; }
  Occupation getPlayer() const                { return player_; }

  std::string const& getPlayerName() const    { return playerName_; }

  // Connect to the process in whatever way is appropriate
  virtual bool establishConnection(const std::vector<std::string>& gameData)   { return true; }

  // Notification that we have moved to the next frame
  virtual void stateUpdated() = 0;

  // Get a player's move for this frame.  Returning an illegal move indicates no changes were requested.  This function blocks execution until data is available.
  virtual Move getMove() = 0;

  // Get all pending player moves.  This function should not block execution -- when no moves are available it should do nothing.
  virtual void getMoves(std::vector<Move>& moves) = 0;
};


// Player used for testing game predicates -- it makes no valid moves
class PlayerDoNothing : public PlayerProxy
{
public:
  PlayerDoNothing(Game& game) :
    PlayerProxy(game)
  {
    playerName_ = "empty";
  }

  void stateUpdated()                         {}
  Move getMove()                              { return Move::Invalid; }
  void getMoves(std::vector<Move>& moves)     {}
};



// External program using sockets for communication
class PlayerSocket : public PlayerProxy
{
  static int listeningSocket_;

  const unsigned short port_;
  int socketHandle_;
  SocketIO io_;


public:

  PlayerSocket(Game& game, unsigned short port);
  ~PlayerSocket();

  bool establishConnection(const std::vector<std::string>& gameData);
  void setPlayer(Occupation player);

  void stateUpdated();
  Move getMove();
  void getMoves(std::vector<Move>& moves);


  static void cleanup()   { Network::platformSpecificCloseSocket(listeningSocket_); }
};
