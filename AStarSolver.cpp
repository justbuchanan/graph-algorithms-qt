#include "AStarSolver.h"

#include <algorithm>
#include <cmath>

AStarSolver::AStarSolver(const StateSpace *ss, State start, State goal)
    : Solver(ss, start, goal) {
  reset();
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
  State c = *std::min_element(_openSet.begin(), _openSet.end(),
                              [this](State s1, State s2) {
                                float f1 = fScore(s1);
                                float f2 = fScore(s2);

                                if (std::abs(f1 - f2) < 0.0001) {
                                  // tie breaker: choose the node closest to the
                                  // goal
                                  return heuristic_cost_estimate(s1, goal) <
                                         heuristic_cost_estimate(s2, goal);
                                } else {
                                  return f1 < f2;
                                }
                              });

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
    _openSet.insert(neighbor);

    float tentative_gScore = gScore(c) + stateSpace->distBetween(c, neighbor);

    if (tentative_gScore >= gScore(neighbor)) {
      // this is not a better path
      continue;
    }

    _cameFrom[neighbor] = c;
    _gScore[neighbor] = tentative_gScore;
    _fScore[neighbor] =
        tentative_gScore + heuristic_cost_estimate(neighbor, goal);
  }
}

std::vector<State> AStarSolver::reconstructPath() {
  std::vector<State> path;

  State current = goal;

  path.push_back(current);

  while (_cameFrom.find(current) != _cameFrom.end()) {
    current = _cameFrom[current];
    path.push_back(current);
  }

  // path.reverse(); TODO

  return path;
}

float AStarSolver::heuristic_cost_estimate(State a, State b) {
  // return abs(a.x - b.x) + abs(a.y - b.y);
  // return stateSpace->distBetween(a, b);

  int dx = std::abs(a.x - b.x);
  int dy = std::abs(a.y - b.y);

  int diag = std::min(dx, dy);
  int straight = dx + dy - 2 * diag;

  return straight * 1 + diag * sqrtf(2.0);
}

bool AStarSolver::hasExplored(State s) const {
  return std::find(_closedSet.begin(), _closedSet.end(), s) != _closedSet.end();
}

void AStarSolver::reset() {
  _openSet.clear();
  _closedSet.clear();
  _cameFrom.clear();
  _fScore.clear();
  _gScore.clear();

  _openSet.insert(start);
  _gScore[start] = 0;
  _fScore[start] = heuristic_cost_estimate(start, goal);
}
