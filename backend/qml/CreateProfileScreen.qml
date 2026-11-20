import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#858585"

    function goBack() {
        StackView.view.pop();
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 500)
        height: 330
        color: "#FFFFFF"
        border.color: "#000000"
        border.width: 2

        Column {
            anchors.fill: parent
            anchors.margins: 30
            spacing: 20

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "CREATE NEW USER"
                color: "#000000"
                font.pixelSize: 22
                font.bold: true
            }

            Text {
                text: "Choose a name for your player profile."
                color: "#404040"
                font.pixelSize: 13
            }

            TextField {
                id: nameField
                width: parent.width
                height: 48
                placeholderText: "Enter username"
                font.pixelSize: 14
                background: Rectangle {
                    color: "#FFFFFF"
                    border.color: "#000000"
                    border.width: 2
                }
            }

            Row {
                anchors.right: parent.right
                spacing: 12

                BrutalButton {
                    text: "CANCEL"
                    buttonColor: "#FFFFFF"
                    implicitWidth: 118
                    implicitHeight: 44
                    onClicked: root.goBack()
                }

                BrutalButton {
                    text: "CREATE"
                    buttonColor: "#FFEA2A"
                    implicitWidth: 118
                    implicitHeight: 44
                    onClicked: root.goBack()
                }
            }
        }
    }
}
