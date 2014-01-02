import QtQuick 2.0
import Sailfish.Silica 1.0

// our class defines this
//import Flakes 1.0
//import Qt3D 2.0
//import Qt3D.Shapes 2.0

Page {
    id: page

    Column {
        anchors.fill: parent

        Button {
            text:"Cube"
            onClicked: pageStack.push("CubePage.qml")
        }
        Button {
            text:"Flakes"
            onClicked: pageStack.push("FlakesPage.qml")
        }
        Button {
            text:"Swarm"
            onClicked: pageStack.push("SwarmPage.qml")
        }
    }

}
