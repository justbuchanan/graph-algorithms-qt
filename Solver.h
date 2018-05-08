#pragma once

#include "StateSpace.h"

class Solver {
public:
  Solver(const StateSpace *ss, State start, State goal)
      : _stateSpace(ss), _start(start), _goal(goal) {}

  virtual void step() = 0;
  virtual std::vector<State> reconstructPath() = 0;
  virtual bool hasExplored(State s) const = 0;

  virtual void reset() = 0;

  virtual void setStateSpace(const StateSpace *ss) {
    _stateSpace = ss;
    reset();
  }

  const StateSpace *stateSpace() const { return _stateSpace; }

  virtual void setStart(State s) {
    _start = s;
    reset();
  }

  virtual void setGoal(State g) {
    _goal = g;
    reset();
  }

  State goal() const { return _goal; }
  State start() const { return _start; }

  bool done() const { return hasExplored(goal()); }

protected:
  const StateSpace *_stateSpace;
  State _start, _goal;
};
