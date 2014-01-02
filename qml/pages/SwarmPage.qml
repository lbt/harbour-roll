import QtQuick 2.0
import Sailfish.Silica 1.0

// our class defines this
import Swarm 1.0
//import Qt3D 2.0
//import Qt3D.Shapes 2.0

Page {
    id: page

    Item {
        anchors.fill: parent

        // Note that a GLItem is always rendered on top of anything else.
        Swarm {
            anchors.fill: blue
            t: slide.sliderValue
            running : true
        }
        Rectangle {
            id: blue
            x:50
            y:50
            width:parent.width - 40
            height:600
            color: "black"
            MouseArea {
                anchors.fill: parent
                drag.target: parent
                drag.minimumX: 0
                drag.maximumX: page.width - parent.width
                drag.minimumY: 0
                drag.maximumY: page.height - parent.height
                }
        }
        Slider {
            id: slide
            anchors.bottom: label.top
            width: parent.width
            minimumValue: -40
            maximumValue: 10
            value: -3
            valueText: value
        }
        Rectangle {
            color: Qt.rgba(1, 1, 1, 0.7)
            radius: 10
            border.width: 1
            border.color: "white"
                anchors.fill: label
//            anchors.margins: -10
//            anchors.right: parent.right
//            anchors.left: parent.left
//            anchors.bottom: parent.bottom
//            height: 200
        }
        Text {
            id: label
            color: "black"
            wrapMode: Text.WordWrap
            text: "The Blue square is a Swarm (subclass of GLItem) rendered with raw OpenGL using the 'afterRender()' signal in QQuickWindow."
            font.pixelSize: Theme.fontSizeMedium
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 20
        }
    }
}
