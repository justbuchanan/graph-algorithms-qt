#pragma once

#include "StateSpace.h"

class Solver {
public:
  Solver(const StateSpace *ss, State start, State goal)
      : stateSpace(ss), start(start), goal(goal) {}

  virtual void step() = 0;
  virtual std::vector<State> reconstructPath() = 0;
  virtual bool hasExplored(State s) const = 0;

  bool done() const { return hasExplored(goal); }

  const StateSpace *stateSpace;
  const State start, goal;
};
