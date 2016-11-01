#include <stdio.h>
#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>

#include "network.h"
#include "playerInterface.h"


PlayerInterface::PlayerInterface() :
  playerAssignment_(Occupation_Total),
  socketHandle_(-1),
  io_(NULL),
  playerIsActive_(false),
  verbose_(false)
{}


PlayerInterface::~PlayerInterface()
{
  delete io_;
  if (socketHandle_ >= 0)
    Network::platformSpecificCloseSocket(socketHandle_);
}


int PlayerInterface::playerProgramEntryPoint(int argc, char* argv[])
{
  // Socket connection to game host

  Network::platformSpecificInitSockets();

  std::string hostname = "localhost";
  std::string port = "8080";
  if (argc >= 2)
  {
    hostname = argv[1];
    size_t portDelimiter = hostname.find_first_of(':');
    if (portDelimiter != std::string::npos)
    {
      port = hostname.substr(portDelimiter + 1);
      hostname = hostname.substr(0, portDelimiter);
    }
  }

  std::cout << "Requesting connection to " << hostname.c_str() << ":" << port.c_str() << std::endl;
  hostent* hostEntity = gethostbyname(hostname.c_str());
  if (!hostEntity)
  {
    std::cerr << "DNS lookup failed" << std::endl;
    std::cerr << std::endl;
    std::cerr << "Usage:  [server_address]" << std::endl;
    std::cerr << "        will use localhost if nothing specified" << std::endl;
    std::cerr << std::endl;
    return 2;
  }

  socketHandle_ = (int)socket(AF_INET, SOCK_STREAM, 0);
  if (socketHandle_ < 0)
  {
    std::cerr << "Unable to create a socket" << std::endl;
    return 2;
  }

  sockaddr_in addr = {0};
  std::memcpy(&addr.sin_addr, hostEntity->h_addr, hostEntity->h_length);
  addr.sin_port = atoi(port.c_str());
  addr.sin_family = AF_INET;

  std::cout << "Attempting to connect to host..." << std::endl;
  if (connect(socketHandle_, (sockaddr*)&addr, sizeof(addr)) < 0)
  {
    std::cerr << "Unable to connect with socket" << std::endl;
    return 2;
  }

  std::cout << "Socket connection established." << std::endl;

  io_ = new SocketIO(socketHandle_);
  io_->setEchoRecvStrings(verbose_);
  io_->setEchoSendStrings(verbose_);

  // Tell the host who we are
  io_->send(Network::versionString);
  io_->send(argv[0]);

  // Get game information from the host
  std::vector<std::string> boardData;
  for (;;)
  {
    std::string line = io_->recv();
    if (line.empty())
      return 3;
    if (line.find_first_of("BOARD:") == 0)
    {
      boardData.push_back(line.substr(6));
    }
    if (line.find_first_of("PLAYER:") == 0)
    {
      // Player assignment marks the end of the board dump
      playerAssignment_ = charToOccupation(line[7]);
      if (playerAssignment_ >= Occupation_Total)
      {
        std::cerr << "Unrecognized player assignment: " << line[7] << std::endl;
        return false;
      }
      std::cout << "Player assignment is " << occupationToChar(playerAssignment_) << std::endl;
      break;
    }
  }

  // Load the game
  if (!game_.loadFile(boardData))
  {
    std::cerr << "Error loading board" << std::endl;
    return 1;
  }

  std::cout << std::endl;
  std::cout << "Game loaded!" << std::endl;
  std::cout << std::endl;

  // Spool up a thread to handle networking
  playerIsActive_ = true;
  std::thread listenThread(hostLiaisonThreadEntryPoint, this);

  // Play!
  playGame();

  // Clean up
  playerIsActive_ = false;
  listenThread.join();
  return 0;
}


void PlayerInterface::makeAMove(Move const& move)
{
  // Once play begins, this is the only place where we send data to the host
  const size_t frameNum = getGame().getBoards().size() - 1;
  std::stringstream moveStream;
  moveStream << "MOVE" << " " << frameNum <<  " " << move.locX_ << " " << move.locY_;
  io_->send(moveStream.str());
}


void PlayerInterface::hostLiaison()
{
  while (playerIsActive_ && !game_.isOver())
  {
    // Wait for data from the  host
    std::string line = io_->recv();
    if (line.empty())
    {
      // There was a receive problem
      exit(0);
    }

    // Look for move notifications; anything else is unexpected
    std::stringstream ss(line);
    std::string command;
    ss >> command;
    if (command != "MOVES")
      break;

    // Check the frame number
    const size_t expectedFrame = getGame().getBoards().size() - 1;
    int frameNum = -1;
    ss >> frameNum;
    if (verbose_)
      std::cout << "Got move notification for frame " << frameNum << std::endl;
    if (frameNum != (int)expectedFrame)
    {
      std::cerr << "Frame mismatch!  Host sent moves for frame " << frameNum << ", we expected frame " << expectedFrame << std::endl;
      return;
    }

    // Parse the individual moves
    std::vector<Move> moves;
    for (;;)
    {
      int x = -1, y = -1;
      ss >> x >> y;

      // Save only the valid moves
      if (x < 0 || x >= (int)game_.getBoardWidth() || y < 0 || y >= (int)game_.getBoardHeight())
        break;

      if (verbose_)
        std::cout << "  Got move (" << x << ", " << y << ") --> " << std::endl;
      moves.push_back(Move(x, y));
    }

    // Apply the moves
    game_.applyMovesAndGenerateNextFrame(moves);
    nextFrame();
  }
}
