#pragma once

#include "Solver.h"
#include "StateSpace.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtQuick/QQuickPaintedItem>
#include <memory>

class GraphWidget : public QQuickPaintedItem {
  Q_OBJECT

public Q_SLOTS:
  void tickTock();

  void useDijkstra();
  void useAstar();
  void useRandom();

public:
  GraphWidget();

  Q_PROPERTY(int iterations READ iterations WRITE setIterations NOTIFY stepped)
  int iterations() const { return _iterations; }
  void setIterations(int i) { _iterations = i; }
  void incItr() { setIterations(iterations() + 1); }

Q_SIGNALS:
  void stepped();

protected:
  void paint(QPainter *painter) override;

  void restartTimer();
  void restartRun();

  // mouse
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  State _stateForPos(QPointF qp);

  static bool mouseInGrabbingRange(QMouseEvent *event, State s);

private:
  template <class T> std::unique_ptr<T> make_solver() const {
    return std::unique_ptr<T>(new T(&_stateSpace, _startPt, _goalPt));
  }

  // if you click down on an obstacle, you enter erase mode.
  // if you click down where there's no obstacle, you enter draw mode.
  bool _editingObstacles, _erasingObstacles;

  enum {
    DraggingNone = 0,
    DraggingStart,
    DraggingGoal,
  } _draggingItem;

  StateSpace _stateSpace;
  std::unique_ptr<Solver> _solver;
  State _startPt, _goalPt;
  QTimer _stepTimer;
  int _iterations = 0;
};
