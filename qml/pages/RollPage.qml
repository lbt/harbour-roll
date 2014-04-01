import QtQuick 2.0
import Sailfish.Silica 1.0
import ".."
import harbour.roll.Roll 1.0

Page {                                          id: page
    property bool runWhenMinimised: coverS.checked
    onRunWhenMinimisedChanged: console.debug("runWhenMinimised " + runWhenMinimised)
    Item {
        anchors.bottom: parent.bottom
        width: parent.width
        height: page.height - menu.height
        anchors.bottomMargin: panel.margin
        Roll {                                 id: roll
            anchors.fill: parent
            running : applicationActive || runWhenMinimised
            MouseArea {                         id: bgMA
                anchors.fill: parent
                preventStealing: true
                Connections {
                    onPositionChanged: roll.handlePositionChanged(mouse.x,mouse.y)
                    onPressed: roll.handlePressed(mouse.x,mouse.y)
                    onReleased: roll.handleReleased(mouse.x,mouse.y)
                }
            }
        }
    }

    Text { id: shake
        color: "white";
        text: "\n\nRoll, roll, roll your boat...!"
        wrapMode:Text.WrapAtWordBoundaryOrAnywhere
        font.pixelSize: Theme.fontSizeHuge
        horizontalAlignment: Text.AlignHCenter
        width: parent.width
        anchors.centerIn: parent
    }
    Item {                                  id: menu
        anchors.top:parent.top
        width: parent.width
        PageHeader {                            id: hdr
            width: parent.width
            anchors.top: parent.top
            title:"Roll"
            MouseArea { anchors.fill: parent
                onClicked: { panel.open = !panel.open }
            }
            visible: page.isLandscape
        }
    }

    DockedPanel {                           id: panel
        // This is the height of the panel. It does not clip.
        // When opened the dock will slide this far to display contents
        // When nesting a flickable, ensure it clips to avoid overdrawing.
        height: page.isPortrait ? parent.height * 0.5 : parent.height
        width:  page.isPortrait ? parent.width : parent.width/2
        dock: page.isPortrait ? Dock.Top : Dock.Left
        open: true // Will close automatically
        visible: applicationActive
        PageHeader {title: "Roll"
            anchors.top:panelFlick.bottom
            anchors.topMargin: 0
            width:panelFlick.width; visible: page.isPortrait
            MouseArea { anchors.fill: parent
                onClicked: { panel.open = !panel.open }
            }
        }
        Rectangle {
            anchors.fill : parent
            color: "black"
            opacity: 0.5
        }

        Image {
            source: "glow_top.png"
            anchors.horizontalCenter: panelFlick.horizontalCenter
            anchors.bottom: panelFlick.top
            anchors.bottomMargin: 2
            width: panelFlick.width/2
            opacity:panelFlick.atYBeginning ? 0 : 0.5
            Behavior on opacity { NumberAnimation {} }
        }
        Image {
            source: "glow_bottom.png"
            anchors.horizontalCenter: panelFlick.horizontalCenter
            anchors.top: panelFlick.bottom
            anchors.topMargin: 2
            width: panelFlick.width/2
            opacity:panelFlick.atYEnd ? 0 : 0.5
            Behavior on opacity { NumberAnimation {} }
        }
        SilicaFlickable {                       id: panelFlick
            pixelAligned: false // remove this post update4
            anchors.fill: parent
            anchors.topMargin: 10
            anchors.bottomMargin: 10
            clip: true
            contentHeight: panelCol.height
            Column { id: panelCol
                //                top: parent.top
                width: parent.width
                Button {text: "Help and About"
                    onClicked: pageStack.push(Qt.resolvedUrl("About.qml"));
                    width: parent.width*0.8; anchors.horizontalCenter: parent.horizontalCenter
                }
                Flow {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Repeater {
                        model: roll.names
                        Button { text: "Add a " + modelData
                            onClicked: roll.addRoll(modelData)
                            anchors.margins: Theme.paddingMedium
                        }
                    }
                }
                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {                    id: fancy
                        text: "Fancy Lights"
                        width: parent.width/2
                        onClicked: roll.fancyLights(checked);
                    }
                    Button {text: "Change colours"
                        onClicked: roll.randomiseLights();
                        width: parent.width/2
                    }
                }
                TextSwitch {                    id: main
                    text: "Main Light"
                    width: parent.width
                    checked: roll.mainLight
                    onClicked: roll.setMainLight(checked)
                }
                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {                    id: spin
                        text: "Fly mode"
                        width: parent.width/2
                        onClicked: roll.zoomAndSpin(checked);
                    }
                    TextSwitch {                    id: gravity
                        text: "Gravity"
                        width: parent.width/2
                        checked: true
                        onClicked: roll.gravity(checked);
                    }
                }
                TextSwitch {                    id: coverS
                    text: "Active when minimised"
                    width: parent.width
                    checked: false
                }
                TextSwitch {                    id: dbg
                    text: "Debug Draw"
                    width: parent.width
                    checked: false
                    onClicked: roll.setDebugDraw(checked);
                }
            }
        }

        SequentialAnimation {id: showMenuAnimation
            PauseAnimation {duration:2000}
            PropertyAction { target: panel; property: "open"; value: false }
            PauseAnimation {duration:500}
            NumberAnimation { target: shake; property: "opacity"; to: 0; duration: 500 }
            PropertyAction { target: shake; property: "visible"; value: false }
        }
    }
    Component.onCompleted:showMenuAnimation.start()
}
