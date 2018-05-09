#pragma once

#include <cmath>
#include <iostream>
#include <vector>

struct State {
  int x, y;

  bool operator==(const State &other) const {
    return x == other.x && y == other.y;
  }

  bool operator!=(const State &other) const { return !(*this == other); }
};

std::ostream &operator<<(std::ostream &os, const State &s);
bool operator<(const State &a, const State &b);

// Maintains obstacles in a 2d discrete state space.
class StateSpace {
public:
  StateSpace(int w, int h);
  virtual ~StateSpace();

  bool obstacleAt(State s) const { return _backingArray[s.x + _w * s.y]; }
  bool &obstacleAt(State s) { return _backingArray[s.x + _w * s.y]; }
  void setBlocked(State s, bool blocked = true) { obstacleAt(s) = blocked; }

  int width() const { return _w; }
  int height() const { return _h; }

  bool inBounds(State s) const {
    return s.x >= 0 && s.x < _w && s.y >= 0 && s.y < _h;
  }

  // Returns all neighbors that are in bounds and not blocked by obstacles.
  std::vector<State> neighborsOf(State s) const;

  float distance(State a, State b) const {
    return sqrtf(powf(a.x - b.x, 2) + powf(a.y - b.y, 2));
  }

private:
  int _w, _h;
  bool *_backingArray;
};
