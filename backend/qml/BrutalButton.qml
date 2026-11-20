import QtQuick
import QtQuick.Controls

Button {
    id: control

    property color buttonColor: "#FFEA2A"
    property color labelColor: "#000000"

    implicitWidth: 180
    implicitHeight: 52
    padding: 0

    contentItem: Text {
        text: control.text
        color: control.enabled ? control.labelColor : "#737373"
        font.family: "Arial"
        font.pixelSize: 15
        font.bold: true
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
    }

    background: Item {
        Rectangle {
            x: 5
            y: 5
            width: parent.width - 5
            height: parent.height - 5
            color: control.enabled ? "#000000" : "#A3A3A3"
        }

        Rectangle {
            width: parent.width - 5
            height: parent.height - 5
            color: control.enabled ? control.buttonColor : "#E5E5E5"
            border.color: control.enabled ? "#000000" : "#A3A3A3"
            border.width: 2
        }
    }
}
