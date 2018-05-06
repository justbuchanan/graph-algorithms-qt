#pragma once

#include "Solver.h"

class RandomSolver : public Solver {
public:
  RandomSolver(const StateSpace *ss, State start, State goal);

  void step() override;
  bool hasExplored(State s) const override;

private:
  std::vector<State> _explored;
};
