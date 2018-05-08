#include "RandomSolver.h"

#include <algorithm>
#include <iostream>

RandomSolver::RandomSolver(const StateSpace *ss, State start, State goal)
    : Solver(ss, start, goal) {
  reset();
}

void RandomSolver::step() {
  const State s = _explored[rand() % _explored.size()];

  auto neighbors = stateSpace()->neighborsOf(s);
  State neighbor = neighbors[rand() % neighbors.size()];

  if (!hasExplored(neighbor)) {
    _explored.push_back(neighbor);
  }
}

bool RandomSolver::hasExplored(State s) const {
  return std::find(_explored.begin(), _explored.end(), s) != _explored.end();
}

void RandomSolver::reset() {
  _explored.clear();
  _explored.push_back(start());
}