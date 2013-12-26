import QtQuick 2.0
import Sailfish.Silica 1.0

// our class defines this
import OpenGLUnderQML 1.0
//import Qt3D 2.0
//import Qt3D.Shapes 2.0

Page {
    id: page

    Item {
        anchors.fill: parent
        // To enable PullDownMenu, place our content in a SilicaFlickable
        //    SilicaFlickable {
        //        anchors.fill: parent

        //        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        //        PullDownMenu {
        //            MenuItem {
        //                text: "Show Page 2"
        //                onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
        //            }
        //        }
        // Qt3D stuff
        //        Viewport {
        //            anchors.right: parent.right
        //            anchors.left: parent.left
        //            anchors.top: parent.top
        //            anchors.bottom: label.top
        //            //width: parent.width; height: parent.height
        //            fillColor: "darkblue"
        //            renderMode: "DirectRender"
        //            objectName: "cube viewport"
        //            Cube {
        //                scale: 1.5
        //                objectName: "cube"
        //                transform: Rotation3D {
        //                    angle: 45
        //                    axis: Qt.vector3d(1, 1, 1)
        //                }
        //                effect: Effect {
        //                    color: "#aaca00"
        //                    texture: "../../cube.png"
        //                    decal: true
        //                }
        //            }
        //        }

        // This Item is not rendered; it simply draws into the window
        Squircle {
            SequentialAnimation on t {
                NumberAnimation { to: 1; duration: 2500; easing.type: Easing.InQuad }
                NumberAnimation { to: 0; duration: 2500; easing.type: Easing.OutQuad }
                loops: Animation.Infinite
                running: true
            }
        }
        Rectangle {
            color: Qt.rgba(1, 1, 1, 0.7)
            radius: 10
            border.width: 1
            border.color: "white"
            anchors.fill: label
            anchors.margins: -10
        }
        Text {
            id: label
            color: "black"
            wrapMode: Text.WordWrap
            text: "The background here is a squircle rendered with raw OpenGL using the 'beforeRender()' signal in QQuickWindow. This text label and its border is rendered using QML"
            font.pixelSize: Theme.fontSizeMedium
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            anchors.margins: 20
        }
    }
}
