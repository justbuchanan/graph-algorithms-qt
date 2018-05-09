#pragma once

#include "Solver.h"
#include <map>
#include <set>

class DijkstraSolver : public Solver {
public:
  DijkstraSolver(const StateSpace *ss, State start, State goal);

  void step() override;
  std::vector<State> reconstructPath() override;
  bool hasExplored(State s) const override;

protected:
  float tentativeDist(State s) const;

private:
  std::set<State> _unvisited;
  std::map<State, float> _tentativeDist;
  std::map<State, State> _prev;
};
