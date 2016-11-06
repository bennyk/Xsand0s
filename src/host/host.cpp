#include "../game/game.h"
#include "../game/network.h"
#include "../game/platform.h"
#include "playerProxy.h"

#include <chrono>
#include <thread>
#include <array>


class ArgHandler
{
  std::vector<std::string> switches_;
  std::vector<std::string> files_;

public:
  ArgHandler(int argc, char* argv[])
  {
    for (int i = 1; i < argc; ++i)
    {
      if (!argv[i])
        continue;
      std::string entry(argv[i]);
      if (entry.empty())
        continue;

      // Handle switch delimiters
      const size_t contentLoc = entry.find_first_not_of('-');
      if (contentLoc == 0)
      {
        files_.push_back(entry);
      }
      else
      {
        if (contentLoc < entry.size())
          switches_.push_back(entry.substr(contentLoc));
      }
    }
  }

  std::vector<std::string> const& getSwitches() const { return switches_; }
  std::vector<std::string> const& getFiles() const    { return files_; }
};


int main(int argc, char* argv[])
{
  ArgHandler argHandler(argc, argv);
  if (argHandler.getFiles().empty())
  {
    std::cerr << "Usage:  [options] <board_file> [player_X player_O]" << std::endl;
    std::cerr << "        -interactive  Timing is disabled; the user controls frame stepping" << std::endl;
    std::cerr << "        -players=<N>  Specify the number of players that will connect:" << std::endl;
    std::cerr << "                        0 predicate simulation only, no gameplay" << std::endl;
    std::cerr << "                        1 test mode for a single player where the opponent will not make any moves" << std::endl;
    std::cerr << "                        2 (default) normal gameplay mode" << std::endl;
    std::cerr << "        -port=<N>     The network port to use for connections" << std::endl;
    std::cerr << "        -quiet        Strong but silent" << std::endl;
    std::cerr << "        -scoredump    Analysis mode that outputs only the score for player X every frame" << std::endl;
    std::cerr << "        -stream       Don't clear the screen between frames; helpful for streaming to a file" << std::endl;
    std::cerr << "    If player names are specified, only accept connections from players with those names." << std::endl;
    return 1;
  }
  const std::string boardFilename = argHandler.getFiles()[0];

  bool quiet = false;
  bool scoringMode = false;
  bool interactiveMode = false;
  bool streamMode = false;
  unsigned numPlayers = 2;
  unsigned short port = Network::defaultPort;
  for (std::vector<std::string>::const_iterator swIt = argHandler.getSwitches().begin(); swIt != argHandler.getSwitches().end(); ++swIt)
  {
    if (*swIt == "interactive")
    {
      interactiveMode = true;
    }
    if (swIt->find("players=") == 0)
    {
      numPlayers = atoi(swIt->substr(8).c_str());
      if (numPlayers > 2)
      {
        std::cerr << "Invalid number of players" << std::endl;
        return 1;
      }
    }
    if (swIt->find("port=") == 0)
    {
      port = (unsigned short)atoi(swIt->substr(5).c_str());
    }
    if (*swIt == "quiet")
    {
      quiet = true;
    }
    if (*swIt == "scoredump")
    {
      scoringMode = true;
      quiet = true;
    }
    if (*swIt == "stream")
    {
      streamMode = true;
    }
  }

  // Handle incompatible options
  if (interactiveMode && scoringMode)
  {
    std::cerr << "Cannot combine -interactive and -scoredump" << std::endl;
    return 1;
  }
  if (interactiveMode)
    quiet = false;

  // Game data...

  Game game;
  if (!quiet)
    std::cout << "Board file is " << boardFilename << std::endl;

  std::vector<std::string> gameData;
  std::ifstream file;
  file.open(boardFilename.c_str());
  if (!file.good())
  {
    std::cerr << "Error loading game parameters" << std::endl;
    return 1;
  }

  while (!file.eof())
  {
    std::string line;
    getline(file, line);
    if (!line.empty())
      gameData.push_back(line);
  }

  if (!game.loadFile(gameData))
  {
    std::cerr << "Error loading board state" << std::endl;
    return 1;
  }

  // Players...

  Network::platformSpecificInitSockets();

  std::vector<PlayerProxy*> players;
  for (unsigned i = 0; i < numPlayers; ++i)
    players.push_back(new PlayerSocket(game, port));
  while (players.size() < 2)
    players.push_back(new PlayerDoNothing(game));

  // Establish the connections
  Occupation playerAssignments[] = { Occupation_PLAYER_X, Occupation_PLAYER_O };
  for (size_t p = 0; p < players.size(); ++p)
  {
    if (!quiet)
      std::cout << "Connecting to player " << occupationToChar(playerAssignments[p]) << "..." << std::endl;
    if (!players[p]->establishConnection(gameData))
      return 2;
  }

  if (!quiet)
    std::cout << "Players ready." << std::endl;

  // Announcing the player assignments triggers the start of the game
  for (size_t p = 0; p < players.size(); ++p)
  {
    players[p]->setPlayer(playerAssignments[p]);
  }

  // Simulation...

  if (!quiet)
    std::cout << "Begin game." << std::endl;

  // Disable line buffering on stdin
  Platform::disableLineBuffering();

  size_t currentBoardIndex = 0;
  std::array<int, Occupation_Total> wins = {0, 0};

  for (bool keepGoing = true; keepGoing;)
  {
    const Board& b = game.getBoards()[currentBoardIndex];
    const bool gameOver = (currentBoardIndex == game.getNumFrames());
    bool moveToNextFrame = false;

    if (!quiet)
    {
      if (!streamMode)
        Platform::clearScreen();
      game.printBoard(currentBoardIndex);

      std::cout << std::endl;
      std::cout << "Frame " << currentBoardIndex << " / " << game.getNumFrames() << std::endl;
      std::cout << "Game: " << game.getPredicate().getDescriptiveName() << std::endl;
      std::cout << "Score:  X = " << b.getScoreForPlayer(Occupation_PLAYER_X) << "  O = " <<  b.getScoreForPlayer(Occupation_PLAYER_O) << std::endl;

      if (b.getScoreForPlayer(Occupation_PLAYER_X) > b.getScoreForPlayer(Occupation_PLAYER_O)) {
        wins[Occupation_PLAYER_X] += 1;
      } else {
        wins[Occupation_PLAYER_O] += 1;
      }

      std::cout << "Wins:  X = " << wins[Occupation_PLAYER_X] << "  O = " <<  wins[Occupation_PLAYER_O] << std::endl;
      if (gameOver)
        std::cout << "--- GAME OVER ---" << std::endl;
    }
    else if (scoringMode)
    {
      // Print just the score for player X
      std::cout << b.getScoreForPlayer(Occupation_PLAYER_X) << std::endl;
      moveToNextFrame = true;
    }

    if (interactiveMode)
    {
      std::cout << std::endl << "n = next frame, p = previous frame, q = quit" << std::endl;
      char c = Platform::getCharNoBuffering();

      switch (c)
      {
      case 'n':
      case ' ':
        if (gameOver)
          break;
        if (currentBoardIndex + 1 == game.getBoards().size())
          moveToNextFrame = true;
        else
          ++currentBoardIndex;
        break;

      case 'p':
        if (currentBoardIndex > 0)
          --currentBoardIndex;
        break;

      case 'q':
        keepGoing = false;
        break;
      }
    }
    else
    {
      // Automatic mode...

      if (gameOver)
      {
        keepGoing = false;
      }
      else
      {
        if (numPlayers)
        {
          // Wait for the frame to elapse
          std::this_thread::sleep_for(std::chrono::milliseconds((int)(1000*game.getMoveTimeLimitInSeconds())));
        }

        moveToNextFrame = true;
      }
    }

    if (moveToNextFrame)
    {
      // Grab the moves made by each player
      std::vector<Move> moves;
      for (std::vector<PlayerProxy*>::iterator pit = players.begin(); pit != players.end(); ++pit)
      {
        std::vector<Move> playerMoves;
        (*pit)->getMoves(playerMoves);
        if (!quiet)
        {
          for (std::vector<Move>::iterator mit = playerMoves.begin(); mit != playerMoves.end(); ++mit)
            std::cout << "Player " << occupationToChar((*pit)->getPlayer()) << " sent move: ("
              << mit->locX_ << ", " << mit->locY_ << ")" << std::endl;
        }

        // Commit the last valid move
        for (std::vector<Move>::reverse_iterator mit = playerMoves.rbegin(); mit != playerMoves.rend(); ++mit)
        {
          if (mit->isValid())
          {
            if (!quiet)
              std::cout << "Committing move (" << mit->locX_ << ", " << mit->locY_ << ")" << std::endl;

            moves.push_back(*mit);
            break;
          }
        }
      }

      // Update simulation
      game.applyMovesAndGenerateNextFrame(moves);

      // Inform the players of the outcome; this triggers the next frame
      for (std::vector<PlayerProxy*>::iterator pit = players.begin(); pit != players.end(); ++pit)
      {
        (*pit)->stateUpdated();
      }

      ++currentBoardIndex;
    }
  }

  if (!scoringMode)
  {
    std::cout << "BOARD " << boardFilename << std::endl;
    for (std::vector<PlayerProxy*>::iterator pit = players.begin(); pit != players.end(); ++pit)
    {
      const Occupation playerOcc = (*pit)->getPlayer();
      std::cout << "PLAYER " << occupationToChar(playerOcc) << " " << (*pit)->getPlayerName()
        << " " << game.getCurrentBoard().getScoreForPlayer(playerOcc) << std::endl;
    }
  }

  // Cleanup
  for (std::vector<PlayerProxy*>::iterator pit = players.begin(); pit != players.end(); ++pit)
  {
    delete *pit;
  }
  players.clear();
  PlayerSocket::cleanup();

  return 0;
}

