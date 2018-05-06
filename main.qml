import QtQuick 2.5
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.2

import GraphWidget 1.0

ApplicationWindow {
    title: "Graph Algorithms"

    // makes window floating by default in tiling window managers
    modality: Qt.WindowModal

    ColumnLayout {
        anchors.fill: parent;

        // main toolbar
        Row{
            Layout.alignment: Qt.AlignTop

            Button {
                text: "Dijkstra"
                onClicked: graphWidget.useDijkstra()
            }

            Button {
                text: "A*"
                onClicked: graphWidget.useAstar()
            }

            Button {
                text: "Random"
                onClicked: graphWidget.useRandom()
            }
        }

        // draw graph
        GraphWidget {
            // Fixed size
            Layout.maximumHeight: 600
            Layout.maximumWidth: 800
            Layout.minimumHeight: 600
            Layout.minimumWidth: 800

            id: graphWidget
        }
    }

    // bottom bar
    statusBar: StatusBar {
        RowLayout {
            id: statusBarLayout
            anchors.fill: parent

            Label {
                text: "Iterations: " + graphWidget.iterations
                anchors.right: statusBarLayout.right
            }
        }
    }
}
