#include "GraphWidget.h"

#include "AStarSolver.h"
#include "DijkstraSolver.h"
#include "RandomSolver.h"
#include <QtGui/QPainter>
#include <cmath>
#include <iostream>

using namespace std;

// TODO: reconcile these with fixed dimensions set in .qml file
static const int kWidth = 100;
static const int kHeight = 100;
static const float kScale = 10;

namespace {
void blockLine(StateSpace *ss, State start, State dt, int t) {
  for (int _t = 0; _t < t; _t++) {
    ss->obstacleAt(State{start.x + _t * dt.x, start.y + _t * dt.y}) = true;
  }
}
} // namespace

GraphWidget::GraphWidget() : _stateSpace(kWidth, kHeight), _stepTimer(this) {
  // setFixedSize(kWidth * kScale, kHeight * kScale);

  _startPt = State{5, 5};
  _goalPt = State{70, 35};

  // obstacles
  _stateSpace.setBlocked(State{2, 20});
  blockLine(&_stateSpace, State{0, 4}, {1, 0}, 4);
  blockLine(&_stateSpace, State{12, 15}, State{1, 0}, 30);
  blockLine(&_stateSpace, State{60, 20}, State{0, 1}, 30);

  // timer to step the algorithm
  connect(&_stepTimer, SIGNAL(timeout()), this, SLOT(tickTock()));

  // start out with A*
  useAstar();

  //  register for mouse events
  setAcceptedMouseButtons(Qt::LeftButton);
  _draggingItem = DraggingNone;
  _editingObstacles = false;
}

void GraphWidget::restartRun() {
  setIterations(0);
  Q_EMIT stepped();
  restartTimer();
}

void GraphWidget::restartTimer() { _stepTimer.start(100); }

void GraphWidget::useDijkstra() {
  _solver = make_solver<DijkstraSolver>();
  restartTimer();
}

void GraphWidget::useAstar() {
  _solver = make_solver<AStarSolver>();
  restartTimer();
}

void GraphWidget::useRandom() {
  _solver = make_solver<RandomSolver>();
  restartTimer();
}

void GraphWidget::tickTock() {
  incItr();
  cout << "itr: " << _iterations << endl;
  if (_solver->done()) {
    cout << "Done!" << endl;
    _stepTimer.stop();
    return;
  }

  // cout << "tick tock" << endl;
  _solver->step();

  update();
}

void GraphWidget::paint(QPainter *painter) {
  // painter.setRenderHint(QPainter::Antialiasing);

  for (int x = 0; x < _stateSpace.width(); x++) {
    for (int y = 0; y < _stateSpace.height(); y++) {
      const State st = {x, y};
      // bool empty = false;
      QColor c = QColor("white");
      if (st == _goalPt) {
        c = QColor("green");
      } else if (st == _startPt) {
        c = QColor("blue");
      } else if (_stateSpace.obstacleAt(st)) {
        c = QColor("black");
      } else if (_solver->hasExplored(st)) {
        c = QColor("yellow");
      }

      painter->setBrush(c);
      painter->setPen(c);

      painter->drawRect(QRectF(x * kScale, y * kScale, kScale, kScale));
    }
  }
}

bool GraphWidget::mouseInGrabbingRange(QMouseEvent *event, State pt) {
  double dx = event->pos().x() / kScale - pt.x;
  double dy = event->pos().y() / kScale - pt.y;
  return sqrtf(dx * dx + dy * dy) < 2;
}

State GraphWidget::_stateForPos(QPointF qp) {
  return State{(int)(qp.x() / kScale), (int)(qp.y() / kScale)};
}

void GraphWidget::mousePressEvent(QMouseEvent *event) {
  if (mouseInGrabbingRange(event, _startPt)) {
    _draggingItem = DraggingStart;
  } else if (mouseInGrabbingRange(event, _goalPt)) {
    _draggingItem = DraggingGoal;
  } else {
    _editingObstacles = true;
    QPointF pos = {(qreal)event->pos().x(), (qreal)event->pos().y()};
    State s = _stateForPos(pos);
    _erasingObstacles = _stateSpace.obstacleAt(s);

    //  toggle the obstacle state of clicked square
    _stateSpace.obstacleAt(s) = !_erasingObstacles;
    update();
  }

  _stepTimer.stop();
}

void GraphWidget::mouseMoveEvent(QMouseEvent *event) {
  const State point = _stateForPos(event->pos());

  if (_draggingItem == DraggingStart) {
    //  reset the tree with the new start pos
    _startPt = point;
  } else if (_draggingItem == DraggingGoal) {
    //  set the new goal point
    _goalPt = point;
  } else if (_editingObstacles) {
    if (point.y >= 0 && point.y < kHeight && point.x >= 0 && point.x < kWidth) {
      _stateSpace.setBlocked(point, !_erasingObstacles);
    }
  }

  if (_draggingItem != DraggingNone || _editingObstacles)
    update();
}

void GraphWidget::mouseReleaseEvent(QMouseEvent *event) {
  _draggingItem = DraggingNone;
  _editingObstacles = false;

  restartTimer();
}
