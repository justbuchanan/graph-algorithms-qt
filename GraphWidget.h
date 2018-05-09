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
  void step();

  void useDijkstra();
  void useAstar();
  void useRandom();

  void clearObstacles();

public:
  GraphWidget();

  Q_PROPERTY(int iterations READ iterations WRITE setIterations NOTIFY stepped)
  int iterations() const { return _iterations; }
  void setIterations(int i) { _iterations = i; }
  void incItr();

Q_SIGNALS:
  void stepped();

protected:
  void paint(QPainter *painter) override;

  void geometryChanged(const QRectF &newGeometry,
                       const QRectF &oldGeometry) override;

  void restartTimer();
  void restartRun();

  // mouse
  void mousePressEvent(QMouseEvent *event);
  void mouseMoveEvent(QMouseEvent *event);
  void mouseReleaseEvent(QMouseEvent *event);

  State _stateForPos(QPointF qp);

private:
  template <class T>
  std::unique_ptr<T> make_solver(State start, State goal) const {
    return std::unique_ptr<T>(new T(_stateSpace.get(), start, goal));
  }

  // if you click down on an obstacle, you enter erase mode.
  // if you click down where there's no obstacle, you enter draw mode.
  bool _editingObstacles, _erasingObstacles;

  enum {
    DraggingNone = 0,
    DraggingStart,
    DraggingGoal,
  } _draggingItem;

  std::unique_ptr<StateSpace> _stateSpace;
  std::unique_ptr<Solver> _solver;
  QTimer _stepTimer;
  int _iterations = 0;

  std::vector<State> _solutionPath;
  bool _solved = false;
};
