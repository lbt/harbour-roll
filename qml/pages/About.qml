import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    Rectangle {
        anchors.fill: parent
        color: "black"
    }
    PageHeader { id: hdr
        anchors.top: parent.top
        width: parent.width
        title: "About Dice"
    }
    SilicaFlickable {                       id: helpFlick
        anchors.top:hdr.bottom
        width: parent.width
        anchors.bottom:parent.bottom
        contentHeight: col.height // required
        Column { id: col
            width: parent.width
            spacing: 5
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Dice started as way to learn how to do 3D graphics on Sailfish. At some point"+
                      " I thought it may be a fun app so here it is. Enjoy!  --  lbt<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Turning on the 'Fancy Lights' makes the lights move around. They're given random" +
                      " colours and movement so tap on 'Change Colours' to randomise them.<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "The 'Zoom and spin mode' lets you move around." +
                      " Touch in the bottom half of the screen to zoom in and out and side-to-side;" +
                      " touch in the top half and you can rotate the dice. Turning off zoom and spin" +
                      " will reset the dice position.<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Turning gravity off makes the dice float around - most fun if you do it"+
                      " after a little shake<p>"
            }

        }
    }
}
