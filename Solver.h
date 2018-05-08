#pragma once

#include "StateSpace.h"

class Solver {
public:
  Solver(const StateSpace *ss, State start, State goal)
      : stateSpace(ss), start(start), goal(goal) {}

  virtual void step() = 0;
  virtual std::vector<State> reconstructPath() = 0;
  virtual bool hasExplored(State s) const = 0;

  virtual void reset() = 0;

  virtual void setStateSpace(const StateSpace *ss) {
    stateSpace = ss;
    reset();
  }

  virtual void setStart(State s) {
    start = s;
    reset();
  }

  virtual void setGoal(State g) {
    goal = g;
    reset();
  }

  bool done() const { return hasExplored(goal); }

protected:
  const StateSpace *stateSpace;
  State start, goal;
};
