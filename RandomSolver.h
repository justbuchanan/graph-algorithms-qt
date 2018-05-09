#pragma once

#include "Solver.h"

class RandomSolver : public Solver {
public:
  RandomSolver(const StateSpace *ss, State start, State goal);

  void step() override;
  std::vector<State> reconstructPath() override {
    // unimplemented
    return {};
  }
  bool hasExplored(State s) const override;

private:
  std::vector<State> _explored;
};
