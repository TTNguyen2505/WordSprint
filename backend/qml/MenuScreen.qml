pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#F4F4F0"

    function openScreen(fileName) {
        // StackView.view is an attached property on the current screen.
        // qmllint disable unqualified
        StackView.view.push(Qt.resolvedUrl(fileName));
        // qmllint enable unqualified
    }

    Column {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 16

        Row {
            width: parent.width
            height: 58
            spacing: 12

            Row {
                width: 210
                height: parent.height
                spacing: 10

                Rectangle {
                    width: 48
                    height: 48
                    anchors.verticalCenter: parent.verticalCenter
                    color: "#FFEA2A"
                    border.color: "#000000"
                    border.width: 2

                    Text {
                        anchors.centerIn: parent
                        text: "W"
                        color: "#000000"
                        font.pixelSize: 24
                        font.bold: true
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    spacing: 3

                    Text {
                        text: "WORDSPRINT"
                        color: "#000000"
                        font.pixelSize: 16
                        font.bold: true
                        font.letterSpacing: 1
                    }

                    Text {
                        text: "PLAYER 01"
                        color: "#737373"
                        font.pixelSize: 10
                        font.bold: true
                        font.letterSpacing: 1
                    }
                }
            }

            Item {
                width: parent.width - 210 - 122 - 156 - 36
                height: 1
            }

            BrutalButton {
                text: "PROFILE"
                buttonColor: "#FFFFFF"
                implicitWidth: 122
                implicitHeight: 46
                onClicked: root.openScreen("ProfileScreen.qml")
            }

            BrutalButton {
                text: "HALL OF FAME"
                buttonColor: "#FCE7F3"
                implicitWidth: 156
                implicitHeight: 46
                onClicked: root.openScreen("LeaderboardScreen.qml")
            }
        }

        Column {
            width: parent.width
            spacing: 10

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "SCRAMBLE"
                color: "#000000"
                font.pixelSize: 36
                font.bold: true
                font.letterSpacing: 2
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "Unscramble the letters. Find the word. Beat the clock."
                color: "#404040"
                font.pixelSize: 14
            }

            BrutalButton {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "START SPRINT"
                buttonColor: "#FFEA2A"
                implicitWidth: 190
                implicitHeight: 48
                onClicked: root.openScreen("CountdownScreen.qml")
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10

                BrutalButton {
                    text: "SCRAMBLE"
                    buttonColor: "#00E599"
                    implicitWidth: 140
                    implicitHeight: 38
                    font.pixelSize: 11
                }

                BrutalButton {
                    text: "FILL IN THE BLANK"
                    buttonColor: "#FFFFFF"
                    implicitWidth: 170
                    implicitHeight: 38
                    font.pixelSize: 11
                }

                BrutalButton {
                    text: "AUDIO"
                    buttonColor: "#FF3B30"
                    labelColor: "#FFFFFF"
                    implicitWidth: 110
                    implicitHeight: 38
                    font.pixelSize: 11
                }
            }
        }

        Row {
            width: parent.width
            height: 30

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "CHOOSE A TOPIC"
                color: "#000000"
                font.pixelSize: 13
                font.bold: true
                font.letterSpacing: 1
            }

            Item {
                width: parent.width - 148
                height: 1
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "VIEW ALL  >"
                color: "#000000"
                font.pixelSize: 11
                font.bold: true
            }
        }

        Row {
            width: parent.width
            spacing: 18

            Repeater {
                model: [
                    {
                        number: "01",
                        topic: "Everyday Words",
                        level: "A1 BEGINNER",
                        status: "SOLVED",
                        score: "850 PTS",
                        accent: "#D1FAE5"
                    },
                    {
                        number: "02",
                        topic: "Food & Kitchen",
                        level: "A1 BEGINNER",
                        status: "AVAILABLE",
                        score: "400 PTS",
                        accent: "#FCE7F3"
                    },
                    {
                        number: "03",
                        topic: "Around Town",
                        level: "A2 ELEMENTARY",
                        status: "AVAILABLE",
                        score: "0 PTS",
                        accent: "#FEF9C3"
                    }
                ]

                delegate: Rectangle {
                    id: stageDelegate

                    required property var modelData
                    width: (parent.width - 36) / 3
                    height: 300
                    color: "#FFFFFF"
                    border.color: "#000000"
                    border.width: 2

                    Column {
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10

                        Row {
                            width: parent.width

                            Text {
                                text: "STAGE " + stageDelegate.modelData.number
                                color: "#000000"
                                font.pixelSize: 10
                                font.bold: true
                                font.letterSpacing: 1
                            }

                            Item {
                                width: parent.width - 130
                                height: 1
                            }

                            Text {
                                text: stageDelegate.modelData.status
                                color: stageDelegate.modelData.status === "SOLVED" ? "#007A52" : "#A83F76"
                                font.pixelSize: 9
                                font.bold: true
                            }
                        }

                        Rectangle {
                            width: parent.width
                            height: 128
                            color: stageDelegate.modelData.accent
                            border.color: "#000000"
                            border.width: 2

                            Rectangle {
                                anchors.centerIn: parent
                                width: 92
                                height: 72
                                color: "#FFFFFF"
                                border.color: "#000000"
                                border.width: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: stageDelegate.modelData.number
                                    color: "#000000"
                                    font.pixelSize: 30
                                    font.bold: true
                                }
                            }
                        }

                        Text {
                            text: stageDelegate.modelData.topic
                            color: "#000000"
                            font.pixelSize: 16
                            font.bold: true
                        }

                        Text {
                            text: stageDelegate.modelData.level
                            color: "#737373"
                            font.pixelSize: 10
                            font.bold: true
                            font.letterSpacing: 1
                        }

                        Row {
                            width: parent.width
                            height: 38

                            Text {
                                width: 80
                                anchors.verticalCenter: parent.verticalCenter
                                text: stageDelegate.modelData.score
                                color: "#000000"
                                font.pixelSize: 11
                                font.bold: true
                                elide: Text.ElideRight
                            }

                            Item {
                                width: parent.width - 80 - 112
                                height: 1
                            }

                            BrutalButton {
                                text: stageDelegate.modelData.status === "SOLVED" ? "REPLAY" : "PLAY"
                                buttonColor: stageDelegate.modelData.status === "SOLVED" ? "#FFFFFF" : "#FFEA2A"
                                implicitWidth: 112
                                implicitHeight: 36
                                font.pixelSize: 10
                                onClicked: root.openScreen("CountdownScreen.qml")
                            }
                        }
                    }
                }
            }
        }
    }
}
