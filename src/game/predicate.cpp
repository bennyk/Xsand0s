#include "predicate.h"
#include "game.h"


Predicate* Predicate::createNamedPredicate(const std::string& name)
{
  if (name == "XsAndOs" || name == "xsandos")
    return new PredicateContest;

  return NULL;
}


void Predicate::destroyNamedPredicate(Predicate* predicate)
{
  delete predicate;
}


double Predicate::currentScore(Game const& game, Occupation playerPOV) const
{
  // Default scoring -- no history
  if (game.getBoards().empty())
    return 0;

  Board const& board = game.getCurrentBoard();
  double score = 0;
  for (int y = 0; y < (int)board.getHeight(); ++y)
  {
    for (int x = 0; x < (int)board.getWidth(); ++x)
    {
      const Occupation occ = board.getLocation(x, y);
      if (occ == playerPOV)
        score += 1;
      else if (occ == oppositePlayer(playerPOV))
        score -= 1;
    }
  }
  return score;
}


int Predicate::calculateDeltaXOinDonut(Game const& game, int x, int y, unsigned framesFromEnd, int donutSize)
{
  int delta = 0;
  for (int j = -donutSize; j <= donutSize; ++j)
  {
    for (int i = -donutSize; i <= donutSize; ++i)
    {
      if (i || j)
      {
        const Occupation occ = game.getLocation(x + i, y + j, framesFromEnd);
        delta += (occ == Occupation_PLAYER_X);
        delta -= (occ == Occupation_PLAYER_O);
      }
    }
  }
  return delta;
}


unsigned Predicate::countNeighborsOfOneType(Game const& game, int x, int y, unsigned framesFromEnd, Occupation playerToCount, int distanceFromCenter, bool countOriginCell)
{
  unsigned count = 0;
  for (int j = -distanceFromCenter; j <= distanceFromCenter; ++j)
  {
    for (int i = -distanceFromCenter; i <= distanceFromCenter; ++i)
    {
      if (countOriginCell || i || j)
      {
        const Occupation occ = game.getLocation(x + i, y + j, framesFromEnd);
        count += (occ == playerToCount);
      }
    }
  }
  return count;
}


///////////////////////////////////////////////////////////


Occupation PredicateContest::calculate(int x, int y, Game const& game) const
{
  const Occupation currentOccupation = game.getLocation(x, y, 0);
  const unsigned numNeighbors = countNeighborsOfOneType(game, x, y, 0, currentOccupation, 1, false);
  switch (numNeighbors)
  {
  case 2:
  case 3:
  case 5:
  case 6:
    return currentOccupation;
  default:
    return oppositePlayer(currentOccupation);
  }
}
