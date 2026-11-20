import QtQuick

Rectangle {
    id: root

    color: "#FEF9C3"

    function openScreen(fileName) {
        StackView.view.push(Qt.resolvedUrl(fileName));
    }

    function returnToMenu() {
        const stack = StackView.view;
        while (stack.depth > 1)
            stack.pop();
    }

    ScreenHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 28
        title: "SPRINT COMPLETE"
        subtitle: "NICE WORK - KEEP IT GOING"
        onBackRequested: root.returnToMenu()
    }

    Column {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 600)
        spacing: 22

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: 110
            height: 110
            color: "#00E599"
            border.color: "#000000"
            border.width: 3
            Text {
                anchors.centerIn: parent
                text: "A+"
                color: "#000000"
                font.pixelSize: 38
                font.bold: true
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Great sprint!"
            color: "#000000"
            font.pixelSize: 40
            font.bold: true
        }

        Rectangle {
            width: parent.width
            height: 126
            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 2

            Row {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 28

                Column {
                    width: (parent.width - 56) / 3
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "SCORE"
                        color: "#737373"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "850"
                        color: "#000000"
                        font.pixelSize: 25
                        font.bold: true
                    }
                }
                Column {
                    width: (parent.width - 56) / 3
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "CORRECT"
                        color: "#737373"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "8 / 10"
                        color: "#000000"
                        font.pixelSize: 25
                        font.bold: true
                    }
                }
                Column {
                    width: (parent.width - 56) / 3
                    anchors.verticalCenter: parent.verticalCenter
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "BEST STREAK"
                        color: "#737373"
                        font.pixelSize: 11
                        font.bold: true
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "5"
                        color: "#000000"
                        font.pixelSize: 25
                        font.bold: true
                    }
                }
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 16

            BrutalButton {
                text: "PLAY AGAIN"
                buttonColor: "#FFEA2A"
                onClicked: root.openScreen("GameScreen.qml")
            }

            BrutalButton {
                text: "LEADERBOARD"
                buttonColor: "#00E599"
                onClicked: root.openScreen("LeaderboardScreen.qml")
            }
        }
    }
}
