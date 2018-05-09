#include "GraphWidget.h"

#include "AStarSolver.h"
#include "DijkstraSolver.h"
#include "RandomSolver.h"
#include <QtGui/QPainter>
#include <cmath>
#include <iostream>

using namespace std;

namespace {

static const float kSquareSize = 20;
static const float kDrawingInset = kSquareSize / 2;

static const QColor kBackgroundColor = Qt::white;

void blockLine(StateSpace *ss, State start, State dt, int t) {
  for (int _t = 0; _t < t; _t++) {
    State s{start.x + _t * dt.x, start.y + _t * dt.y};
    if (ss->inBounds(s)) {
      ss->obstacleAt(s) = true;
    }
  }
}

// If @pt is in bounds, returns @pt. Otherwise returns the nearest point that is
// within the bounds of the given state space.
State nearestInBounds(const StateSpace &ss, State pt) {
  if (ss.inBounds(pt)) {
    return pt;
  }

  pt.x = std::min(pt.x, ss.width() - 1);
  pt.y = std::min(pt.y, ss.height() - 1);

  return pt;
}

void copyObstacles(const StateSpace &from, StateSpace *to) {
  for (int x = 0; x < std::min(from.width(), to->width()); x++) {
    for (int y = 0; y < std::min(from.height(), to->height()); y++) {
      State s{x, y};
      to->obstacleAt(s) = from.obstacleAt(s);
    }
  }
}

class SolverFactory {
public:
  virtual Solver *create(StateSpace *stateSpace, State start, State goal) = 0;
};

template <class SOLVER> class SolverFactoryImpl : public SolverFactory {
public:
  Solver *create(StateSpace *stateSpace, State start, State goal) override {
    return new SOLVER(stateSpace, start, goal);
  }
};

std::unique_ptr<Solver> createSolver(const string &name, StateSpace *stateSpace,
                                     State start, State goal) {
  static std::map<string, SolverFactory *> solvers = {
      {"astar", new SolverFactoryImpl<AStarSolver>()},
      {"dijkstra", new SolverFactoryImpl<DijkstraSolver>()},
      {"random", new SolverFactoryImpl<RandomSolver>()},
  };

  return std::unique_ptr<Solver>(
      solvers[name]->create(stateSpace, start, goal));
}

} // namespace

GraphWidget::GraphWidget()
    : _stateSpace(new StateSpace(100, 100)), _stepTimer(this) {
  //  register for mouse events
  setAcceptedMouseButtons(Qt::LeftButton);
  _draggingItem = DraggingNone;
  _editingObstacles = false;

  _solved = false;

  // obstacles
  blockLine(_stateSpace.get(), State{12, 15}, State{1, 0}, 30);
  blockLine(_stateSpace.get(), State{60, 20}, State{0, 1}, 30);

  // timer to step the algorithm
  connect(&_stepTimer, SIGNAL(timeout()), this, SLOT(step()));

  // start out with A* and arbitrary start and end points
  _useNamedSolver("astar", {5, 5}, {30, 20});
}

void GraphWidget::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry) {

  // skip invalid geometry - this happens when the app is first launching.
  if (newGeometry.width() < 0.0001 || newGeometry.height() < 0.0001) {
    return;
  }

  // Use the largest grid we can fit in the new dimensions.
  const int w = floor((newGeometry.width() - kDrawingInset * 2) / kSquareSize);
  const int h = floor((newGeometry.height() - kDrawingInset * 2) / kSquareSize);

  // Create a state space with the new dimensions
  StateSpace *newStateSpace = new StateSpace(w, h);
  copyObstacles(*_stateSpace, newStateSpace);
  _stateSpace.reset(newStateSpace);

  // move start and goal to be in bounds
  State start = nearestInBounds(*newStateSpace, _solver->start);
  State goal = nearestInBounds(*newStateSpace, _solver->goal);

  // reset solver
  _useNamedSolver(_currentSolverName, start, goal);
}

void GraphWidget::restartRun() {
  _solutionPath.clear();
  _solved = false;
  setIterations(0);
  restartTimer();
}

void GraphWidget::restartTimer() {
  // Note: the parameter can be adjusted to change the speed of the animation.
  _stepTimer.start(10);
}

void GraphWidget::_useNamedSolver(string name) {
  _useNamedSolver(name, _solver->start, _solver->goal);
}

void GraphWidget::_useNamedSolver(string name, State start, State goal) {
  _currentSolverName = name;
  _solver = createSolver(_currentSolverName, _stateSpace.get(), start, goal);
  restartRun();
}

void GraphWidget::useDijkstra() { _useNamedSolver("dijkstra"); }

void GraphWidget::useAstar() { _useNamedSolver("astar"); }

void GraphWidget::useRandom() { _useNamedSolver("random"); }

void GraphWidget::clearObstacles() {
  _stateSpace->clearObstacles();
  restartRun();
}

void GraphWidget::setIterations(int i) {
  if (i != _iterations) {
    _iterations = i;
    Q_EMIT stepped();
  }
}

void GraphWidget::incItr() {
  setIterations(iterations() + 1);
  Q_EMIT stepped();
}

void GraphWidget::step() {
  incItr();
  if (_solver->done()) {
    // Done
    _solutionPath = _solver->reconstructPath();
    _solved = true;
    _stepTimer.stop();
  } else {
    _solver->step();
  }

  update();
}

void GraphWidget::paint(QPainter *painter) {
  painter->setRenderHint(QPainter::Antialiasing);

  // background white
  painter->setBrush(kBackgroundColor);
  painter->drawRect(QRectF{0, 0, width(), height()});

  painter->translate(QPointF{kDrawingInset, kDrawingInset});
  for (int x = 0; x < _stateSpace->width(); x++) {
    for (int y = 0; y < _stateSpace->height(); y++) {
      const State st = {x, y};
      QColor c = QColor("#f1f1f1");
      bool empty = true;
      if (st == _solver->goal) {
        c = QColor("#4CAF50"); // green
        empty = false;
      } else if (st == _solver->start) {
        c = QColor("#F44336"); // red
        empty = false;
      } else if (_stateSpace->obstacleAt(st)) {
        c = QColor(Qt::black);
        empty = false;
      } else if (_solved &&
                 std::find(_solutionPath.begin(), _solutionPath.end(), st) !=
                     _solutionPath.end()) {
        c = QColor("#2196F3"); // blue
        empty = false;
      } else if (_solver->hasExplored(st)) {
        c = QColor("#FFEB3B"); // yellow
        empty = false;
      }

      const QPointF center =
          QPointF((x + 0.5) * kSquareSize, (y + 0.5) * kSquareSize);

      if (empty) {
        painter->setBrush(Qt::black);

        QPen pen(Qt::black);
        pen.setWidth(1);
        painter->setPen(pen);

        painter->drawEllipse(center, 1, 1);
        continue;
      } else {
        painter->setBrush(c);
        painter->setPen(c);

        float rad = kSquareSize / 2.5;
        painter->drawEllipse(center, rad, rad);
      }
    }
  }
}

State GraphWidget::_stateForPos(QPointF qp) {
  return State{(int)(qp.x() / kSquareSize), (int)(qp.y() / kSquareSize)};
}

void GraphWidget::mousePressEvent(QMouseEvent *event) {
  QPointF pos = event->pos() - QPointF(kDrawingInset, kDrawingInset);
  State state = _stateForPos(pos);

  if (state == _solver->start) {
    _draggingItem = DraggingStart;
  } else if (state == _solver->goal) {
    _draggingItem = DraggingGoal;
  } else {
    _editingObstacles = true;
    _erasingObstacles = _stateSpace->obstacleAt(state);

    //  toggle the obstacle state of clicked square
    _stateSpace->obstacleAt(state) = !_erasingObstacles;
    update();
  }

  _stepTimer.stop();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event) {
  const State point =
      _stateForPos(event->pos() - QPointF(kDrawingInset, kDrawingInset));

  if (_draggingItem == DraggingStart) {
    //  reset the tree with the new start pos
    if (_solver->start != point) {
      _useNamedSolver(_currentSolverName, point, _solver->goal);
    }
  } else if (_draggingItem == DraggingGoal) {
    //  set the new goal point
    if (_solver->goal != point) {
      _useNamedSolver(_currentSolverName, _solver->start, point);
    }
  } else if (_editingObstacles) {
    if (_stateSpace->inBounds(point)) {
      _stateSpace->setBlocked(point, !_erasingObstacles);
    }
  }

  _stepTimer.stop(); // stay paused

  if (_draggingItem != DraggingNone || _editingObstacles)
    update();
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event) {
  _draggingItem = DraggingNone;
  _editingObstacles = false;

  restartTimer();
}
