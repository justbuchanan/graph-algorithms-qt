#include "AStarSolver.h"

#include <algorithm>
#include <cmath>

AStarSolver::AStarSolver(const StateSpace *ss, State start, State goal)
    : Solver(ss, start, goal) {
  _openSet.insert(start);
  _gScore[start] = 0;
  _fScore[start] = heuristic_cost_estimate(start, goal);
}

float AStarSolver::fScore(State s) {
  if (_fScore.find(s) == _fScore.end()) {
    return std::numeric_limits<float>::max();
  }

  return _fScore[s];
}

float AStarSolver::gScore(State s) {
  if (_gScore.find(s) == _gScore.end()) {
    return std::numeric_limits<float>::max();
  }

  return _gScore[s];
}

void AStarSolver::step() {
  if (_openSet.size() == 0) {
    std::cout << "done or failed" << std::endl;
    return;
  }

  // find item in openSet with lowest f score
  State c = *std::min_element(
      _openSet.begin(), _openSet.end(),
      [this](State s1, State s2) { return fScore(s1) < fScore(s2); });

  _openSet.erase(c);
  _closedSet.insert(c);

  if (c == goal) {
    std::cout << "Done!" << std::endl;
    return;
  }

  for (auto neighbor : stateSpace->neighborsOf(c)) {
    if (_closedSet.find(neighbor) != _closedSet.end()) {
      continue; // this neighbor already evaluated
    }

    // add to open set if not in already
    if (_openSet.find(neighbor) == _openSet.end()) {
      _openSet.insert(neighbor);
    }

    float tentative_gScore = gScore(c) + stateSpace->distBetween(c, neighbor);

    if (tentative_gScore >= gScore(neighbor)) {
      // this is not a better path
      continue;
    }

    _cameFrom[neighbor] = c;
    _gScore[neighbor] = tentative_gScore;
    _fScore[neighbor] =
        gScore(neighbor) + heuristic_cost_estimate(neighbor, goal);
  }
}

float AStarSolver::heuristic_cost_estimate(State a, State b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
  // return stateSpace->distBetween(a, b);
}

bool AStarSolver::hasExplored(State s) const {
  return std::find(_closedSet.begin(), _closedSet.end(), s) != _closedSet.end();
}
