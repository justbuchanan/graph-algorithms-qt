import QtQuick 2.6
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
            padding: 5
            spacing: 5

            Layout.alignment: Qt.AlignTop

            Button {
                text: "Dijkstra"
                onClicked: graphWidget.useDijkstra()
            }

            Button {
                text: "A*"
                onClicked: graphWidget.useAstar()
            }

            // Button {
            //     text: "Random"
            //     onClicked: graphWidget.useRandom()
            // }
        }

        Row{
            Layout.minimumHeight: 600
            Layout.minimumWidth: 800
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom

            // draw graph
            GraphWidget {
                anchors.fill: parent

                id: graphWidget
            }
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

    // keyboard shortcuts
    Shortcut { sequence: 'a'; onActivated: graphWidget.useAstar() }
    Shortcut { sequence: 'd'; onActivated: graphWidget.useDijkstra() }
    Shortcut { sequence: 'r'; onActivated: graphWidget.useRandom() }
}
