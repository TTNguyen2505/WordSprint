import QtQuick

Rectangle {
    id: root

    color: "#FEF9C3"

    function openScreen(fileName) {
        StackView.view.push(Qt.resolvedUrl(fileName));
    }

    ScreenHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 28
        showBack: false
    }

    Column {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 760)
        spacing: 24

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "READY, SET, LEARN"
            color: "#000000"
            font.family: "Arial"
            font.pixelSize: 14
            font.bold: true
            font.letterSpacing: 3
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Every word\ntakes you further."
            color: "#000000"
            font.family: "Arial"
            font.pixelSize: 54
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            lineHeight: 0.9
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Build your vocabulary one quick challenge at a time."
            color: "#404040"
            font.family: "Arial"
            font.pixelSize: 17
            horizontalAlignment: Text.AlignHCenter
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 18

            BrutalButton {
                text: "CHOOSE A STAGE"
                buttonColor: "#FFEA2A"
                onClicked: root.openScreen("StageSelectionScreen.qml")
            }

            BrutalButton {
                text: "PROFILE"
                buttonColor: "#FFFFFF"
                onClicked: root.openScreen("ProfileScreen.qml")
            }

            BrutalButton {
                text: "LEADERBOARD"
                buttonColor: "#00E599"
                onClicked: root.openScreen("LeaderboardScreen.qml")
            }
        }

        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            width: Math.min(parent.width, 620)
            height: 86
            color: "#FFFFFF"
            border.color: "#000000"
            border.width: 2

            Row {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 36

                Column {
                    Text {
                        text: "CURRENT PLAYER"
                        color: "#000000"
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1
                    }
                    Text {
                        text: "WordSprinter"
                        color: "#000000"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }

                Rectangle {
                    width: 2
                    height: 48
                    color: "#000000"
                }

                Column {
                    Text {
                        text: "TOTAL SCORE"
                        color: "#000000"
                        font.pixelSize: 11
                        font.bold: true
                        font.letterSpacing: 1
                    }
                    Text {
                        text: "1,250 PTS"
                        color: "#000000"
                        font.pixelSize: 16
                        font.bold: true
                    }
                }
            }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 24
        text: "LEARN WORDS. SPRINT AHEAD."
        color: "#404040"
        font.family: "Arial"
        font.pixelSize: 11
        font.bold: true
        font.letterSpacing: 2
    }
}
