import QtQuick 2.0
import Sailfish.Silica 1.0

Page {id: aboutPage
    Rectangle {
        anchors.fill: parent
        color: "black"
    }
    PageHeader { id: hdr
        anchors.top: parent.top
        width: parent.width
        title: "About Dice"
    }
    Image {
        source: "glow_top.png"
        anchors.horizontalCenter: helpFlick.horizontalCenter
        anchors.bottom: helpFlick.top
        anchors.bottomMargin: 2
        width: helpFlick.width/2
        opacity:helpFlick.atYBeginning ? 0 : 0.5
        Behavior on opacity { NumberAnimation {} }
    }
    Image {
        source: "glow_bottom.png"
        anchors.horizontalCenter: helpFlick.horizontalCenter
        anchors.top: helpFlick.bottom
        anchors.topMargin: 2
        width: helpFlick.width/2
        opacity:helpFlick.atYEnd ? 0 : 0.5
        Behavior on opacity { NumberAnimation {} }
    }
    SilicaFlickable {                       id: helpFlick
        anchors.top:hdr.bottom
        anchors.topMargin: 10
        width: parent.width
        anchors.bottom:parent.bottom
        anchors.bottomMargin: 10
        clip: true
        contentHeight: col.height // required
        pixelAligned: false // remove this post update4
        Column { id: col
            width: parent.width
            spacing: 5
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Dice started as a way to learn how to do 3D graphics on Sailfish. At some point"+
                      " I thought it may be a fun app so here it is. Enjoy!  --  lbt<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "If you like this app then please <img src='image://theme/icon-s-like'>" +
                      " it and maybe say something in the" +
                      " store comments - it's nice for us developers to know you're enjoying our work :)<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Check this page each time the app is updated and see what's new. Now, on with"+
                      " what you can do:<p>"
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
                      " will reset the dice position. (I'm working on a much more fun way of moving).<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "Turning gravity off makes the dice float around - most fun if you do it"+
                      " after a little shake or when you change the number of dice.<p>"
            }
            Text {
                color: "white"; width: parent.width; wrapMode:Text.WrapAtWordBoundaryOrAnywhere
                text: "You can see the dice when the app is minimised - and if you like you"+
                      " can even keep it running (and eating your battery!).<p>"
            }
        }
    }
}
