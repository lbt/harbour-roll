import QtQuick 2.0
import Sailfish.Silica 1.0

// our class defines this
import Swarm 1.0

Page {                                          id: page
    allowedOrientations: Orientation.All
    Rectangle {
        //        contentHeight: childrenRect.height
        anchors.bottom: parent.bottom
        width: parent.width
        height: page.height - menu.height
        anchors.bottomMargin: panel.margin
        color: "black"
        //         Note that a GLItem is always rendered on top of anything else.
        Swarm {                                 id: aswarm
            anchors.fill: parent

            depth: dSlide.sliderValue
            numParticles: nSlide.sliderValue
            running : true
            // this may need to be a state which handles animations between 270 and 0 correctly
            orientationInDegrees: (function() { switch(page.orientation) {
                case Orientation.Landscape: return 270;
                case Orientation.PortraitInverted: return 180;
                case Orientation.LandscapeInverted: return 90;
                default: return 0;
                }}())
            Behavior on orientationInDegrees { NumberAnimation {} }
            MouseArea {                         id: bgMA
                anchors.fill: parent
                preventStealing: true
                Connections {
                    onPositionChanged: aswarm.handlePositionChanged(mouse.x,mouse.y)
                    onPressed: aswarm.handlePressed(mouse.x,mouse.y)
                    onReleased: aswarm.handleReleased(mouse.x,mouse.y)
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
            //            anchors.bottom: parent.bottom
            title:"Swarm"
            MouseArea { anchors.fill: parent
                onClicked: {panel.open = !panel.open; console.log("clicked header panel:" + panel.open) }
            }
            visible: page.isLandscape || !panel.open
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
        PageHeader {title: "Swarm"
            anchors.bottom:panelFlick.bottom;
            width:panelFlick.width; visible: page.isPortrait
        }
        Rectangle {color:"green";
            anchors.right:panelFlick.right; width: 30;
            height: panelFlick.height; visible: page.isLandscape
        }

        SilicaFlickable {                       id: panelFlick
            anchors.top:parent.top
            anchors.left:parent.left
            width: page.isPortrait ? parent.width : parent.width - 30
            height: page.isPortrait ? parent.height - 30: parent.height
            clip: true
            contentHeight: panelCol.height
            Column { id: panelCol
                anchors.top: parent.top
                width: parent.width
                Slider {                        id: dSlide
                    width: parent.width
                    minimumValue: -60; maximumValue: 20
                    value: -25
                    valueText: value.toFixed(2);
                }
                Slider {                        id: nSlide
                    width: parent.width
                    minimumValue: 0; maximumValue: 400
                    value: 20
                    stepSize: value <= 10 ? 1 : 10
                    valueText: value
                }
            }
        }
    }
}
