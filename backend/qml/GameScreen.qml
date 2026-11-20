import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#F4F4F0"

    function openScreen(fileName) {
        StackView.view.push(Qt.resolvedUrl(fileName));
    }

    function goBack() {
        StackView.view.pop();
    }

    ScreenHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 28
        title: "EVERYDAY WORDS"
        subtitle: "STAGE 02  /  QUESTION 04 OF 10"
        onBackRequested: root.goBack()
    }

    Column {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 660)
        spacing: 18

        Row {
            width: parent.width
            Text {
                text: "SCRAMBLE"
                color: "#A83F76"
                font.pixelSize: 12
                font.bold: true
                font.letterSpacing: 1
            }
            Item {
                width: parent.width - 200
                height: 1
            }
            Text {
                text: "TIME  00:42"
                color: "#000000"
                font.pixelSize: 13
                font.bold: true
            }
        }

        Rectangle {
            width: parent.width
            height: 210
            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 2
            Text {
                anchors.centerIn: parent
                text: "IMAGE HINT"
                color: "#737373"
                font.pixelSize: 14
                font.bold: true
                font.letterSpacing: 2
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10

            Repeater {
                model: ["A", "P", "P", "L", "E"]
                delegate: Rectangle {
                    required property string modelData
                    width: 58
                    height: 64
                    color: "#FFEA2A"
                    border.color: "#000000"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: modelData
                        color: "#000000"
                        font.pixelSize: 25
                        font.bold: true
                    }
                }
            }
        }

        TextField {
            id: answerField
            width: parent.width
            height: 54
            placeholderText: "Type your answer here..."
            font.pixelSize: 16
            color: "#000000"
            background: Rectangle {
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 2
            }
        }

        Row {
            anchors.right: parent.right
            spacing: 14

            BrutalButton {
                text: "CLEAR"
                buttonColor: "#FFFFFF"
                implicitWidth: 130
                onClicked: answerField.clear()
            }

            BrutalButton {
                text: "SUBMIT"
                buttonColor: "#FFEA2A"
                implicitWidth: 150
                onClicked: root.openScreen("ResultScreen.qml")
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Rearrange the letters to form the word shown in the picture."
            color: "#404040"
            font.pixelSize: 13
        }
    }
}
