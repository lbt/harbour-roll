import QtQuick 2.0
import Sailfish.Silica 1.0
import ".."
import Dice 1.0

Page {                                          id: page
    Item {
        anchors.bottom: parent.bottom
        width: parent.width
        height: page.height - menu.height
        anchors.bottomMargin: panel.margin
        Dice {                                 id: dice
            anchors.fill: parent
            running : applicationActive
            numDice: numDiceS.value
            MouseArea {                         id: bgMA
                anchors.fill: parent
                preventStealing: true
                Connections {
                    onPositionChanged: dice.handlePositionChanged(mouse.x,mouse.y)
                    onPressed: dice.handlePressed(mouse.x,mouse.y)
                    onReleased: dice.handleReleased(mouse.x,mouse.y)
                }
            }
        }
    }

    Item {                                  id: menu
        anchors.top:parent.top
        width: parent.width
        PageHeader {                            id: hdr
            width: parent.width
            anchors.top: parent.top
            title:"Dice"
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
        height: page.isPortrait ? parent.height/2 : parent.height
        width:  page.isPortrait ? parent.width : parent.width/2
        dock: page.isPortrait ? Dock.Top : Dock.Left
        open: false
        PageHeader {title: "Dice"
            anchors.top:panelFlick.bottom
            anchors.topMargin: 20
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

        SilicaFlickable {                       id: panelFlick
            anchors.top:parent.top
            anchors.left:parent.left
            width: page.isPortrait ? parent.width : parent.width - 30
            height: page.isPortrait ? parent.height - 30: parent.height
            clip: true
            contentHeight: panelCol.height
            Column { id: panelCol
//                top: parent.top
                width: parent.width
                Button {text: "Help and About"
                    onClicked: pageStack.push(Qt.resolvedUrl("About.qml"));
                    width: parent.width*0.8; anchors.horizontalCenter: parent.horizontalCenter
                }
                Slider {                        id: numDiceS
                    label: "number of dice"
                    width: parent.width
                    minimumValue: 1; maximumValue: 20
                    stepSize: 1
                    value: 6
                    valueText: value.toFixed(0);
                }
                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {                    id: fancy
                        text: "Fancy Lights"
                        width: parent.width/2
                        onClicked: dice.fancyLights(checked);
                    }
                    Button {text: "Change colours"
                        onClicked: dice.randomiseLights();
                        width: parent.width/2
                    }
                }
                TextSwitch {                    id: spin
                    text: "Zoom and spin mode"
                    width: parent.width
                    onClicked: dice.zoomAndSpin(checked);
                }
                TextSwitch {                    id: gravity
                    text: "Gravity"
                    width: parent.width
                    onClicked: dice.gravity(checked);
                }
//                // d4 d6 d8 d10 d12 d20
//                Row { width: parent.width;height: childrenRect.height
//                    TextSwitch { id: d4; text: "d4"; width: parent.width/3 }
//                    TextSwitch { id: d6; text: "d6"; width: parent.width/3 }
//                    TextSwitch { id: d8; text: "d8"; width: parent.width/3 }}
//                Row { width: parent.width;height: childrenRect.height
//                    TextSwitch { id: d10; text: "d10"; width: parent.width/3 }
//                    TextSwitch { id: d12; text: "d12"; width: parent.width/3 }
//                    TextSwitch { id: d20; text: "d20"; width: parent.width/3 }}
            }
        }
    }
}

