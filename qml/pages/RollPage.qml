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
        Roll {                                 id: track
            anchors.fill: parent
            running : applicationActive || runWhenMinimised
            MouseArea {                         id: bgMA
                anchors.fill: parent
                preventStealing: true
                Connections {
                    onPositionChanged: track.handlePositionChanged(mouse.x,mouse.y)
                    onPressed: track.handlePressed(mouse.x,mouse.y)
                    onReleased: track.handleReleased(mouse.x,mouse.y)
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
                        model: track.names
                        Button { text: "Use track " + modelData
                            onClicked: track.useTrack(modelData)
                            anchors.margins: Theme.paddingMedium
                        }
                    }
                }
                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {                    id: fancy
                        text: "Fancy Lights"
                        width: parent.width/2
                        onClicked: track.fancyLights(checked);
                    }
                    Button {text: "Change colours"
                        onClicked: track.randomiseLights();
                        width: parent.width/2
                    }
                }
                TextSwitch {                    id: main
                    text: "Main Light"
                    width: parent.width
                    checked: track.mainLight
                    onClicked: track.setMainLight(checked)
                }
                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {                    id: spin
                        text: "Fly mode"
                        width: parent.width/2
                        onClicked: track.zoomAndSpin(checked);
                    }
                    TextSwitch {                    id: gravity
                        text: "Gravity"
                        width: parent.width/2
                        checked: true
                        onClicked: track.gravity(checked);
                    }
                }
                Row { width: parent.width; height: childrenRect.height
                    Button {text: "Fly"
                        onClicked: track.setCamera("flycam");
                        width: parent.width/3
                    }
                    Button {text: "Follow"
                        onClicked: track.setCamera("followcam");
                        width: parent.width/3
                    }
                    Button {text: "Curve"
                        onClicked: track.setCamera("curvecam");
                        width: parent.width/3
                    }
                }
                Row { width: parent.width; height: childrenRect.height
                    Button {text: "Top"
                        onClicked: track.setCameraPos(0,-0.1,32);
                        width: parent.width/3
                    }
                    Button {text: "Front"
                        onClicked: track.setCameraPos(0,-32,0);
                        width: parent.width/3
                    }
                    Button {text: "Side"
                        onClicked: track.setCameraPos(32,0,0);
                        width: parent.width/3
                    }
                }
                TextSwitch {                    id: coverS
                    text: "Active when minimised"
                    width: parent.width
                    checked: false
                }

                Row { width: parent.width; height: childrenRect.height
                    TextSwitch {
                        text: "W/F"
                        width: parent.width/3
                        onClicked: track.setDebugDraw(1);
                    }
                    TextSwitch {
                        text: "F-W/F"
                        width: parent.width/3
                        onClicked: track.setDebugDraw((1<<13));
                    }
                    TextSwitch {
                        text: "Nmls"
                        width: parent.width/3
                        onClicked: track.setDebugDraw((1<<14));
                    }
                }


                Flow {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    Repeater {
                        model: track.debugItems
                        Button { text: "Use track " + modelData
                            onClicked: track.toggleDebug(modelData)
                            anchors.margins: Theme.paddingMedium
                        }
                    }
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
