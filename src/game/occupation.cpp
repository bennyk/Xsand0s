#include "occupation.h"


Occupation oppositePlayer(Occupation occ)
{
  switch (occ)
  {
  case Occupation_PLAYER_X:
    return Occupation_PLAYER_O;
  case Occupation_PLAYER_O:
    return Occupation_PLAYER_X;
  default:
    return Occupation_Total;
  }
}


Occupation charToOccupation(char c)
{
  switch (c)
  {
  case 'X':
  case 'x':
    return Occupation_PLAYER_X;
  case 'O':
  case 'o':
    return Occupation_PLAYER_O;
  default:
    return Occupation_Total;
  }
}


char occupationToChar(Occupation occ)
{
  switch (occ)
  {
  case Occupation_PLAYER_X:
    return 'X';
  case Occupation_PLAYER_O:
    return 'O';
  default:
    return ' ';
  }
}
