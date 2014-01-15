import QtQuick 2.0
import Sailfish.Silica 1.0

TextSwitch {
    width: parent.width
    checked: parent.use == text
    automaticCheck: false
    onClicked: parent.useXYZ(text)
}
