import QtQuick

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
        title: "PLAYER PROFILE"
        subtitle: "YOUR WORDSPRINT PROGRESS"
        onBackRequested: root.goBack()
    }

    Column {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 680)
        spacing: 22

        Rectangle {
            width: parent.width
            height: 132
            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 2

            Row {
                anchors.fill: parent
                anchors.margins: 22
                spacing: 20

                Rectangle {
                    width: 78
                    height: 78
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#FFEA2A"
                    border.color: "#000000"
                    border.width: 2
                    Text {
                        anchors.centerIn: parent
                        text: "WS"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 6
                    Text {
                        text: "CURRENT PLAYER"
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1
                    }
                    Text {
                        text: "WordSprinter"
                        font.pixelSize: 24
                        font.bold: true
                    }
                    Text {
                        text: "Member since today"
                        color: "#737373"
                        font.pixelSize: 13
                    }
                }
            }
        }

        Row {
            width: parent.width
            spacing: 18

            Repeater {
                model: [
                    {
                        label: "TOTAL SCORE",
                        value: "1,250"
                    },
                    {
                        label: "STAGES CLEARED",
                        value: "3 / 12"
                    },
                    {
                        label: "BEST STREAK",
                        value: "8 WORDS"
                    }
                ]

                delegate: Rectangle {
                    required property var modelData
                    width: (parent.width - 36) / 3
                    height: 118
                    color: "#FFFFFF"
                    border.color: "#000000"
                    border.width: 2

                    Column {
                        anchors.centerIn: parent
                        spacing: 9
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData.label
                            color: "#737373"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                        }
                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: modelData.value
                            color: "#000000"
                            font.pixelSize: 22
                            font.bold: true
                        }
                    }
                }
            }
        }

        BrutalButton {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "VIEW LEADERBOARD"
            buttonColor: "#00E599"
            onClicked: root.openScreen("LeaderboardScreen.qml")
        }
    }
}
