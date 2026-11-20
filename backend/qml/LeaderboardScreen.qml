import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#858585"

    function goBack() {
        // qmllint disable unqualified
        StackView.view.pop();
        // qmllint enable unqualified
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 620)
        height: 580
        color: "#FFFFFF"
        border.color: "#000000"
        border.width: 2

        Column {
            anchors.fill: parent
            anchors.margins: 28
            spacing: 14

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "HALL OF FAME"
                color: "#000000"
                font.pixelSize: 24
                font.bold: true
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "TOP SPRINTERS"
                color: "#737373"
                font.pixelSize: 10
                font.bold: true
                font.letterSpacing: 1
            }

            Rectangle {
                width: parent.width
                height: 2
                color: "#000000"
            }

            Row {
                width: parent.width
                height: 30

                Text {
                    width: 52
                    anchors.verticalCenter: parent.verticalCenter
                    text: "RANK"
                    color: "#404040"
                    font.pixelSize: 10
                    font.bold: true
                }

                Text {
                    width: parent.width - 220
                    anchors.verticalCenter: parent.verticalCenter
                    text: "PLAYER"
                    color: "#404040"
                    font.pixelSize: 10
                    font.bold: true
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "SCORE"
                    color: "#404040"
                    font.pixelSize: 10
                    font.bold: true
                }
            }

            Column {
                width: parent.width
                spacing: 0

                Repeater {
                    model: [
                        {
                            rank: "01",
                            name: "WordWizard",
                            score: "12,450"
                        },
                        {
                            rank: "02",
                            name: "LexiRunner",
                            score: "10,820"
                        },
                        {
                            rank: "03",
                            name: "WordSprinter",
                            score: "1,250"
                        },
                        {
                            rank: "04",
                            name: "QuickThinker",
                            score: "980"
                        },
                        {
                            rank: "05",
                            name: "Bookworm",
                            score: "760"
                        },
                        {
                            rank: "06",
                            name: "WordSeeker",
                            score: "540"
                        },
                        {
                            rank: "07",
                            name: "Guest",
                            score: "120"
                        }
                    ]

                    delegate: Rectangle {
                        id: leaderboardDelegate

                        required property var modelData
                        width: parent.width
                        height: 43
                        color: leaderboardDelegate.modelData.rank === "03" ? "#FFEA2A" : "#FFFFFF"
                        border.color: "#000000"
                        border.width: 1

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12

                            Text {
                                width: 52
                                anchors.verticalCenter: parent.verticalCenter
                                text: leaderboardDelegate.modelData.rank
                                color: "#000000"
                                font.pixelSize: 11
                                font.bold: true
                            }

                            Text {
                                width: parent.width - 112
                                anchors.verticalCenter: parent.verticalCenter
                                text: leaderboardDelegate.modelData.name
                                color: "#000000"
                                font.pixelSize: 12
                                font.bold: leaderboardDelegate.modelData.rank === "03"
                            }

                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                text: leaderboardDelegate.modelData.score
                                color: "#000000"
                                font.pixelSize: 11
                            }
                        }
                    }
                }
            }

            BrutalButton {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "CLOSE"
                buttonColor: "#FFEA2A"
                implicitWidth: 120
                implicitHeight: 42
                onClicked: root.goBack()
            }
        }
    }
}
