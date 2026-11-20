import QtQuick
import QtQuick.Layouts

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
        title: "CHOOSE YOUR STAGE"
        subtitle: "PICK A TOPIC AND START A SPRINT"
        onBackRequested: root.goBack()
    }

    GridLayout {
        anchors.centerIn: parent
        width: Math.min(parent.width - 80, 980)
        columns: 3
        columnSpacing: 22
        rowSpacing: 22

        Repeater {
            model: [
                {
                    number: "01",
                    topic: "Everyday Words",
                    level: "A1 BEGINNER",
                    state: "SOLVED",
                    score: "850 PTS",
                    fill: "#D1FAE5"
                },
                {
                    number: "02",
                    topic: "Food & Kitchen",
                    level: "A1 BEGINNER",
                    state: "AVAILABLE",
                    score: "400 PTS",
                    fill: "#FCE7F3"
                },
                {
                    number: "03",
                    topic: "Around Town",
                    level: "A2 ELEMENTARY",
                    state: "LOCKED",
                    score: "0 PTS",
                    fill: "#E5E5E5"
                }
            ]

            delegate: Rectangle {
                required property var modelData
                Layout.fillWidth: true
                Layout.preferredHeight: 310
                color: modelData.state === "LOCKED" ? "#E5E5E5" : "#FFFFFF"
                border.color: modelData.state === "LOCKED" ? "#A3A3A3" : "#000000"
                border.width: 2

                Column {
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12

                    Row {
                        width: parent.width
                        Text {
                            text: "STAGE " + modelData.number
                            color: "#000000"
                            font.pixelSize: 11
                            font.bold: true
                            font.letterSpacing: 1
                        }
                        Item {
                            width: parent.width - 168
                            height: 1
                        }
                        Text {
                            text: modelData.state
                            color: modelData.state === "SOLVED" ? "#007A52" : modelData.state === "LOCKED" ? "#737373" : "#A83F76"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 142
                        color: modelData.fill
                        border.color: "#000000"
                        border.width: 2
                        Text {
                            anchors.centerIn: parent
                            text: modelData.number
                            color: "#000000"
                            font.pixelSize: 44
                            font.bold: true
                        }
                    }

                    Column {
                        spacing: 4
                        Text {
                            text: modelData.topic
                            color: "#000000"
                            font.pixelSize: 19
                            font.bold: true
                        }
                        Text {
                            text: modelData.level
                            color: "#737373"
                            font.pixelSize: 11
                            font.bold: true
                            font.letterSpacing: 1
                        }
                    }

                    Row {
                        width: parent.width
                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: modelData.score
                            color: "#000000"
                            font.pixelSize: 12
                            font.bold: true
                        }
                        Item {
                            width: parent.width - 204
                            height: 1
                        }
                        BrutalButton {
                            text: modelData.state === "SOLVED" ? "REPLAY" : modelData.state === "LOCKED" ? "LOCKED" : "PLAY"
                            buttonColor: modelData.state === "SOLVED" ? "#FFFFFF" : "#000000"
                            labelColor: modelData.state === "SOLVED" ? "#000000" : "#FFFFFF"
                            enabled: modelData.state !== "LOCKED"
                            implicitWidth: 118
                            implicitHeight: 42
                            onClicked: root.openScreen("GameScreen.qml")
                        }
                    }
                }
            }
        }
    }
}
