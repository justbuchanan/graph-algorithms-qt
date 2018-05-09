#include "StateSpace.h"

#include <cstring>
#include <memory>

/// State

std::ostream &operator<<(std::ostream &os, const State &s) {
  return os << "State{" << s.x << ", " << s.y << "}";
}

bool operator<(const State &a, const State &b) {
  return a.x < b.x || (a.x == b.x && a.y < b.y);
}

bool operator==(const State &a, const State &b) {
  return a.x == b.x && a.y == b.y;
}

bool operator!=(const State &a, const State &b) { return !(a == b); }

/// StateSpace

StateSpace::StateSpace(int w, int h) : _w(w), _h(h) {
  // TODO: more c++
  _backingArray = (bool *)malloc(sizeof(bool) * w * h);
  clearObstacles();
}

void StateSpace::clearObstacles() {
  memset(_backingArray, 0, _w * _h * sizeof(bool));
}

StateSpace::~StateSpace() { free(_backingArray); }

std::vector<State> StateSpace::neighborsOf(State s) const {
  std::vector<State> ns;
  for (int i = -1; i < 2; i++) {
    for (int j = -1; j < 2; j++) {
      State n{s.x + i, s.y + j};
      if (inBounds(n) && n != s && !obstacleAt(n)) {
        ns.push_back(n);
      }
    }
  }

  return ns;
}
