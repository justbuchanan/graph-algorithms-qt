#pragma once

#include "Solver.h"
#include <map>
#include <set>

class AStarSolver : public Solver {
public:
  AStarSolver(const StateSpace *ss, State start, State goal);

  void step() override;
  bool hasExplored(State s) const override;

protected:
  float fScore(State s);
  float gScore(State s);

  float heuristic_cost_estimate(State a, State b);

private:
  std::set<State> _closedSet;
  std::set<State> _openSet;
  std::map<State, State> _cameFrom;
  // cost of getting from start to that node. Default value infinity
  std::map<State, float> _gScore;

  // Default value infinity
  std::map<State, float> _fScore;
};
