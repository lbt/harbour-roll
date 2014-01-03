import QtQuick 2.0
import Sailfish.Silica 1.0

// our class defines this
import Swarm 1.0

Page {
    id: page

    Item {
        anchors.fill: parent

        // Note that a GLItem is always rendered on top of anything else.
        Swarm {
            id: aswarm
            anchors.fill: bg
            t: slide.sliderValue
            running : true
            x: bgMA.mouseX
            y: bgMA.mouseY
            pressed: bgMA.pressed
        }
        Rectangle {
            id: bg
            anchors.fill: parent
            color: "black"
            MouseArea {
                id: bgMA
                anchors.fill: parent
                preventStealing: true
                //                drag.target: parent
                //                drag.minimumX: 0
                //                drag.maximumX: page.width - parent.width
                //                drag.minimumY: 0
                //                drag.maximumY: page.height - parent.height
                //                Connections {
                //                    target: aswarm;
                //                    onPositionChanged: { aswarm.handlePosition(MouseArea.mouseX); }
            }
        }
        Slider {
            id: slide
            anchors.bottom: label.top
            width: parent.width
            minimumValue: -60
            maximumValue: 20
            value: -25
            valueText: value.toFixed(2);
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
            text: "The page is filled with a Swarm (subclass of GLItem) rendered with raw OpenGL using the 'afterRender()' signal in QQuickWindow."
            font.pixelSize: Theme.fontSizeMedium
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 20
        }
    }
}
