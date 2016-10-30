#include <sstream>

#include "game.h"
#include "board.h"
#include "predicate.h"


Game::Game() :
  numFrames_(1000),
  moveTimeLimitInSeconds_(1),
  numMovesPerFramePerPlayer_(1),
  predicate_(NULL)
{
  boards_.reserve(numFrames_ + 1);
  moves_.reserve(numFrames_ + 1);
}


Game::~Game()
{
  boards_.clear();
  moves_.clear();
  Predicate::destroyNamedPredicate(predicate_);
}


Game::Game(Game const& g) :
    numFrames_(g.numFrames_),
    moveTimeLimitInSeconds_(g.moveTimeLimitInSeconds_),
    numMovesPerFramePerPlayer_(g.numMovesPerFramePerPlayer_),
    predicate_(Predicate::createNamedPredicate(g.predicate_->getFactoryName()))
{
  boards_.reserve(numFrames_ + 1);
  moves_.reserve(numFrames_ + 1);

  // Copy the boards
  for (std::vector<Board>::const_iterator bit = g.boards_.begin(); bit != g.boards_.end(); ++bit)
    boards_.push_back(*bit);

  // Copy the moves
  for (std::vector< std::vector< Move > >::const_iterator mvit = g.moves_.begin(); mvit != g.moves_.end(); ++mvit)
    moves_.push_back(*mvit);
}


bool Game::loadFile(const std::vector<std::string>& lines)
{
  const bool verbose = false;

  // Parse game parameters
  if (verbose)
    std::cout << "Parsing parameters..." << std::endl;
  for (size_t lineNum = 0; lineNum < lines.size(); ++lineNum)
  {
    const std::string& line = lines[lineNum];
    if (verbose)
      std::cout << line << std::endl;

    // Commands are one line each
    std::stringstream ss(line);
    std::string param;
    ss >> param;
    if (param == "frames")
      ss >> numFrames_;
    if (param == "moves")
      ss >> numMovesPerFramePerPlayer_;
    if (param == "time")
      ss >> moveTimeLimitInSeconds_;
    if (param == "predicate")
    {
      std::string predicateName;
      ss >> predicateName;
      predicate_ = Predicate::createNamedPredicate(predicateName);
    }
  }

  // Default predicate
  if (!predicate_)
    predicate_ = Predicate::createNamedPredicate("XsAndOs");

  boards_.clear();
  boards_.reserve(numFrames_);
  moves_.clear();
  moves_.reserve(numFrames_);

  // Read the board
  if (verbose)
    std::cout << "Parsing layout..." << std::endl;

  boards_.resize(1);
  if (!boards_.back().loadFromText(lines))
  {
    boards_.clear();
    return false;
  }

  // Initial score
  boards_.back().setScoreForPlayerX(predicate_->currentScore(*this, Occupation_PLAYER_X));
  return true;
}


Occupation Game::getLocation(int x, int y, unsigned framesFromEnd) const
{
  if (framesFromEnd >= boards_.size())
    return Occupation_Total;
  const size_t boardIndex = boards_.size() - 1 - framesFromEnd;
  return boards_[boardIndex].getLocation(x, y);
}


void Game::printBoard(size_t frameNum) const
{
  if (frameNum == 0)
    getBoards()[0].print(NULL);
  else
    getBoards()[frameNum].print(&getMovesApplied(frameNum - 1));
}


void Game::applyMovesAndGenerateNextFrame(const std::vector<Move>& moves)
{
  if (!predicate_ || boards_.empty() || isOver())
    return;

  // Result storage
  Board b(boards_.back());
  moves_.resize(moves_.size() + 1);

  // Apply the players' moves...

  for (size_t m0 = 0; m0 < moves.size(); ++m0)
  {
    // Range check
    if (moves[m0].locX_ < 0 || moves[m0].locX_ >= (int)getBoardWidth())
      continue;
    if (moves[m0].locY_ < 0 || moves[m0].locY_ >= (int)getBoardHeight())
      continue;

    // Filter out duplicates
    bool moveOkay = true;
    for (size_t m1 = 0; m1 < m0; ++m1)
    {
      if (moves[m0] == moves[m1])
      {
        // Duplicate move
        moveOkay = false;
        break;
      }
    }
    if (!moveOkay)
      continue;

    // Save the move
    moves_.back().push_back(moves[m0]);
    b.applyMove(moves[m0]);
  }

  // Add the board containing the move results so the predicate will pick it up; this is not the final state yet.
  boards_.push_back(b);

  for (unsigned y = 0; y < b.getHeight(); ++y)
  {
    for (unsigned x = 0; x < b.getWidth(); ++x)
    {
      b.setLocation(x, y, predicate_->calculate(x, y, *this));
    }
  }

  // Replace the official board with the results of the simulation
  boards_.back() = b;

  // Calculate the score of the newly generated board.  Note that this has to be done after the board
  // is in the vector so the predicate knows the correct state.
  boards_.back().setScoreForPlayerX(predicate_->currentScore(*this, Occupation_PLAYER_X));
}
