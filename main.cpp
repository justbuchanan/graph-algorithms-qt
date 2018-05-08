#include "GraphWidget.h"
#include <QtQml/QQmlApplicationEngine>
#include <QtQuick/QQuickView>
#include <QtQuick/QtQuick>
#include <QtWidgets/QApplication>
#include <iostream>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setOrganizationName("justbuchanan");
  app.setApplicationName("Graph Algorithms");

  // load qml
  QQmlApplicationEngine engine(nullptr);
  engine.rootContext()->setContextProperty("main", &engine);

  qmlRegisterType<GraphWidget>("GraphWidget", 1, 0, "GraphWidget");

  // load main.qml from qt resources - baked into binary as a QResource
  engine.load((QUrl("qrc:///main.qml")));

  // get reference to main window from qml
  auto win = static_cast<QQuickView *>(engine.rootObjects()[0]);
  if (!win) {
    std::cerr << "Failed to load qml" << std::endl;
    exit(1);
  }

  win->show();

  return app.exec();
}
