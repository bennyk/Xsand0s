#pragma once

#include "occupation.h"

class Game;

class Predicate
{
  std::string factoryName_;
  std::string descriptiveName_;

protected:
  Predicate(const std::string& factoryName, const std::string& descriptiveName) :
    factoryName_(factoryName),
    descriptiveName_(descriptiveName)
  {}

public:
  virtual ~Predicate() {}

  // Short name for board encoding and factory creation
  const std::string& getFactoryName() const       { return factoryName_; }

  // Human-friendly printable name
  const std::string& getDescriptiveName() const   { return descriptiveName_; }


  // Handle custom predicates specified in a game file by name.  Returns NULL if the algorithm is unknown.
  static Predicate* createNamedPredicate(const std::string& name);
  static void destroyNamedPredicate(Predicate* predicate);


  // Determines the next state of the given cell
  virtual Occupation calculate(int x, int y, Game const& game) const = 0;

  // The game score from the specified player's point of view.
  // Note that this calculation may depend on any number of frames in the game, not just the current one.
  // The default implementation simply counts the number of cells owned by the player relative to the opponent.
  virtual double currentScore(Game const& game, Occupation playerPOV) const;

protected:

  // Helper functions
  static int calculateDeltaXOinDonut(Game const& game, int x, int y, unsigned framesFromEnd, int donutSize);
  static unsigned countNeighborsOfOneType(Game const& game, int x, int y, unsigned framesFromEnd, Occupation playerToCount, int distanceFromCenter, bool countOriginCell);
};

// Official 2016 Programming contest
class PredicateContest : public Predicate
{
public:
  PredicateContest() :
    Predicate("XsAndOs", "Programming Contest 2016 rules")  {}

  Occupation calculate(int x, int y, Game const& game) const;
};
