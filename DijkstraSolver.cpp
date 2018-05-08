#include "DijkstraSolver.h"

#include <algorithm>

DijkstraSolver::DijkstraSolver(const StateSpace *ss, State start, State goal)
    : Solver(ss, start, goal) {
  reset();
}

void DijkstraSolver::reset() {
  _tentativeDist.clear();
  _unvisited.clear();
  _prev.clear();

  _tentativeDist[start()] = 0;

  //  mark all nodes unvisited
  for (int x = 0; x < stateSpace()->width(); x++) {
    for (int y = 0; y < stateSpace()->height(); y++) {
      State s{x, y};

      if (!stateSpace()->obstacleAt(s)) {
        _unvisited.insert(s);
      }
    }
  }
}

float DijkstraSolver::tentativeDist(State s) const {
  if (_tentativeDist.find(s) == _tentativeDist.end()) {
    return std::numeric_limits<float>::max();
  }

  return _tentativeDist.find(s)->second;
}

void DijkstraSolver::step() {
  // get unvisited node with smallest tentative dist
  const State current = *std::min_element(
      _unvisited.begin(), _unvisited.end(), [this](State s1, State s2) {
        return tentativeDist(s1) < tentativeDist(s2);
      });

  for (State neighbor : stateSpace()->neighborsOf(current)) {
    if (hasExplored(neighbor)) {
      continue;
    }

    // update tentative dist
    float new_dist =
        _tentativeDist[current] + stateSpace()->distBetween(current, neighbor);
    if (new_dist < tentativeDist(neighbor)) {

      _tentativeDist[neighbor] = new_dist;
      _prev[neighbor] = current;
    }
  }

  _unvisited.erase(current);
}

std::vector<State> DijkstraSolver::reconstructPath() {
  std::vector<State> path;

  State current = goal();
  while (current != start()) {
    path.push_back(current);
    current = _prev[current];
  }

  path.push_back(start());
  // TODO: reverse

  return path;
}

bool DijkstraSolver::hasExplored(State s) const {
  return _unvisited.find(s) == _unvisited.end();
}
