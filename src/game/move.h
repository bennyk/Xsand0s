#pragma once

#include "occupation.h"


struct Move
{
  int locX_, locY_;

  Move(int locX, int locY) :
    locX_(locX),
    locY_(locY)
  {}

  bool operator==(const Move& m) const  { return locX_ == m.locX_ && locY_ == m.locY_; }
  bool isValid() const                  { return locX_ >= 0 && locY_ >= 0; }

  const static Move Invalid;
};
