#include "GraphWidget.h"

#include "AStarSolver.h"
#include "DijkstraSolver.h"
#include "RandomSolver.h"
#include <QtGui/QPainter>
#include <cmath>
#include <iostream>

using namespace std;

static const float kSquareSize = 20;
static const float kDrawingInset = kSquareSize / 2;

namespace {

void blockLine(StateSpace *ss, State start, State dt, int t) {
  for (int _t = 0; _t < t; _t++) {
    State s{start.x + _t * dt.x, start.y + _t * dt.y};
    if (ss->inBounds(s)) {
      ss->obstacleAt(s) = true;
    }
  }
}

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

} // namespace

GraphWidget::GraphWidget()
    : _stateSpace(new StateSpace(100, 100)), _stepTimer(this) {
  _startPt = State{5, 5};
  _goalPt = State{30, 20};

  _solved = false;

  // obstacles
  blockLine(_stateSpace.get(), State{12, 15}, State{1, 0}, 30);
  blockLine(_stateSpace.get(), State{60, 20}, State{0, 1}, 30);

  // timer to step the algorithm
  connect(&_stepTimer, SIGNAL(timeout()), this, SLOT(step()));

  // start out with A*
  useAstar();

  //  register for mouse events
  setAcceptedMouseButtons(Qt::LeftButton);
  _draggingItem = DraggingNone;
  _editingObstacles = false;
}

void GraphWidget::geometryChanged(const QRectF &newGeometry,
                                  const QRectF &oldGeometry) {

  if (newGeometry.width() < 0.0001 || newGeometry.height() < 0.0001) {
    // skip invalid geometry - happens when app is first launching
    return;
  }

  cout << "Resized" << endl;
  cout << newGeometry.width() << endl;

  int w = floor((newGeometry.width() - kDrawingInset * 2) / kSquareSize);
  int h = floor((newGeometry.height() - kDrawingInset * 2) / kSquareSize);

  StateSpace *newStateSpace = new StateSpace(w, h);
  copyObstacles(*_stateSpace, newStateSpace);
  _stateSpace.reset(newStateSpace);

  // move start and goal to be in bounds
  _startPt = nearestInBounds(*newStateSpace, _startPt);
  _goalPt = nearestInBounds(*newStateSpace, _goalPt);

  // update solver
  _solver->setStateSpace(newStateSpace);
  _solver->setGoal(_goalPt);
  _solver->setGoal(_startPt);
  _solver = make_solver<AStarSolver>();

  restartRun();
}

void GraphWidget::restartRun() {
  _solutionPath.clear();
  _solved = false;
  setIterations(0);
  _solver->reset();
  Q_EMIT stepped();
  restartTimer();
}

void GraphWidget::restartTimer() { _stepTimer.start(100); }

void GraphWidget::useDijkstra() {
  _solver = make_solver<DijkstraSolver>();
  restartRun();
}

void GraphWidget::useAstar() {
  _solver = make_solver<AStarSolver>();
  restartRun();
}

void GraphWidget::useRandom() {
  _solver = make_solver<RandomSolver>();
  restartRun();
}
void GraphWidget::incItr() {
  setIterations(iterations() + 1);
  Q_EMIT stepped();
}

void GraphWidget::step() {
  incItr();
  cout << "itr: " << _iterations << endl;
  if (_solver->done()) {
    cout << "Done!" << endl;
    _solutionPath = _solver->reconstructPath();
    _solved = true;
    _stepTimer.stop();
  } else {
    _solver->step();
  }

  update();
}

const QColor kBackgroundColor = Qt::white;

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
      if (st == _goalPt) {
        c = QColor("#4CAF50"); // green
        empty = false;
      } else if (st == _startPt) {
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

  if (state == _startPt) {
    _draggingItem = DraggingStart;
  } else if (state == _goalPt) {
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
    if (_startPt != point) {
      _startPt = point;
      _solver->setStart(_startPt);
      restartRun();
    }
  } else if (_draggingItem == DraggingGoal) {
    //  set the new goal point
    if (_goalPt != point) {
      _goalPt = point;
      _solver->setGoal(_goalPt);
      restartRun();
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
