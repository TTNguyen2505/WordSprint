import QtQuick

Rectangle {
    id: root

    color: "#F4F4F0"

    function goBack() {
        StackView.view.pop();
    }

    ScreenHeader {
        id: header
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 28
        title: "LEADERBOARD"
        subtitle: "TOP SPRINTERS THIS WEEK"
        onBackRequested: root.goBack()
    }

    Column {
        anchors.centerIn: parent
        width: Math.min(parent.width - 64, 720)
        spacing: 10

        Row {
            width: parent.width
            height: 42
            Text {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: "PLAYER"
                color: "#737373"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 1
            }
            Text {
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                text: "TOTAL SCORE"
                color: "#737373"
                font.pixelSize: 11
                font.bold: true
                font.letterSpacing: 1
            }
        }

        Repeater {
            model: [
                {
                    rank: "01",
                    player: "WordWizard",
                    score: "12,450",
                    fill: "#FFEA2A"
                },
                {
                    rank: "02",
                    player: "LexiRunner",
                    score: "10,820",
                    fill: "#FFFFFF"
                },
                {
                    rank: "03",
                    player: "WordSprinter",
                    score: "1,250",
                    fill: "#D1FAE5"
                },
                {
                    rank: "04",
                    player: "QuickThinker",
                    score: "980",
                    fill: "#FFFFFF"
                }
            ]

            delegate: Rectangle {
                required property var modelData
                width: parent.width
                height: 68
                color: modelData.fill
                border.color: "#000000"
                border.width: 2

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 18
                    anchors.rightMargin: 18

                    Text {
                        width: 60
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.rank
                        color: "#000000"
                        font.pixelSize: 15
                        font.bold: true
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.player
                        color: "#000000"
                        font.pixelSize: 16
                        font.bold: true
                    }
                    Item {
                        width: parent.width - 270
                        height: 1
                    }
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: modelData.score + " PTS"
                        color: "#000000"
                        font.pixelSize: 14
                        font.bold: true
                    }
                }
            }
        }
    }
}
