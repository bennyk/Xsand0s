#include <stdio.h>
#include <string.h>
#include <sstream>
#include <signal.h>

#include "../game/network.h"
#include "playerProxy.h"


// Socket used during connection finding; shared among all potential players
int PlayerSocket::listeningSocket_ = -1;


PlayerSocket::PlayerSocket(Game& game, unsigned short port) :
  PlayerProxy(game),
  port_(port),
  socketHandle_(-1),
  io_(-1)
{}


bool PlayerSocket::establishConnection(const std::vector<std::string>& gameData)
{
  const bool echo = false;

  if (listeningSocket_ < 0)
  {
    std::cout << "Setting up to accept connections" << std::endl;

    // Communications socket
    listeningSocket_ = (int)socket(AF_INET, SOCK_STREAM, 0);
    if (listeningSocket_ < 0)
    {
      std::cerr << "Unable to create a socket" << std::endl;
      return false;
    }

    if (1)
    {
      // Allow port re-use
      const char one = 1;
      setsockopt(listeningSocket_, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    }
    // Bind the address
    sockaddr_in addr = {0};
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);
    addr.sin_family = AF_INET;

    if (bind(listeningSocket_, (sockaddr*)&addr, sizeof(addr)) < 0)
    {
      std::cerr << "Unable to bind socket" << std::endl;
      return false;
    }

    std::cout << "Can now listen for connections." << std::endl;
  }

  std::cout << "Waiting for player..." << std::endl;
  listen(listeningSocket_, 2);
  socketHandle_ = (int)accept(listeningSocket_, NULL, NULL);
  if (socketHandle_ < 0)
  {
    std::cerr << "Error accepting connection" << std::endl;
    return false;
  }
  std::cout << "Socket connection established." << std::endl;

  io_.setSocketHandle(socketHandle_);
  io_.setEchoRecvStrings(echo);
  io_.setEchoSendStrings(echo);

  // Check that they are speaking the same language
  std::string communicationVersion = io_.recv();
  if (communicationVersion != Network::versionString)
  {
    std::cerr << "Player is using the wrong communication protocol."
      << "  Expecting: \"" << Network::versionString << "\", got: \"" << communicationVersion << "\"" << std::endl;
    return false;
  }

  // Get the name of the player
  playerName_ = io_.recv();
  if (playerName_.empty())
    return false;
  std::cout << "Communicating with " << playerName_ << std::endl;

  // Send over the game file
  for (size_t l = 0; l < gameData.size(); ++l)
  {
    io_.send("BOARD:" + gameData[l]);
  }

  return true;
}


PlayerSocket::~PlayerSocket()
{
  Network::platformSpecificCloseSocket(socketHandle_);
}


void PlayerSocket::setPlayer(Occupation player)
{
  PlayerProxy::setPlayer(player);

  // Transmit the player assignment
  std::string cmd = "PLAYER:";
  cmd += occupationToChar(player);
  io_.send(cmd);
}


void PlayerSocket::stateUpdated()
{
  // Send the moves

  std::stringstream ss;
  const size_t frameNum = game_.getBoards().size() - 2;  // (The new state has already been added)
  ss << "MOVES " << frameNum << " ";

  const std::vector<Move>& moves = game_.getMovesApplied(frameNum);
  for (std::vector<Move>::const_iterator mit = moves.begin(); mit != moves.end(); ++mit)
  {
    ss << " " << mit->locX_ << " " << mit->locY_;
  }

  io_.send(ss.str());
}


Move PlayerSocket::getMove()
{
  for (;;)
  {
    std::string line = io_.recv();
    if (line.empty())
      break;
    std::stringstream ss(line);
    std::string command;
    ss >> command;

    // Move command?
    if (command != "MOVE")
      continue;

    // Parse it

    const int expectedFrameNum = (int)game_.getBoards().size() - 1;
    int frameNum = -1;
    ss >> frameNum;
    if (frameNum != expectedFrameNum)
    {
      // This move is not relevant to the current frame -- it's probably a stale leftover
      //std::cout << "Frame mismatch.  Bug or just late?  " << frameNum << " expected " << expectedFrameNum << std::endl;
      continue;
    }

    int x = -1, y = -1;
    ss >> x >> y;
    //std::cout << "Got move from player " << occupationToChar(player_) << "... " << x << ", " << y << std::endl;
    if (x < 0 || x >= (int)game_.getBoardWidth() || y < 0 || y >= (int)game_.getBoardHeight())
      break;
    return Move(x, y);
  }

  return Move::Invalid;
}


void PlayerSocket::getMoves(std::vector<Move>& moves)
{
  while (io_.dataAvailable())
  {
    Move m = getMove();
    if (m.isValid())
      moves.push_back(m);
  }
}
