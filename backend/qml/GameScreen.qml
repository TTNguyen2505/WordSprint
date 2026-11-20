pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    color: "#F4F4F0"

    function goBack() {
        StackView.view.pop();
    }

    Column {
        anchors.fill: parent
        anchors.margins: 24
        spacing: 16

        Row {
            width: parent.width
            height: 54
            spacing: 14

            BrutalButton {
                text: "< BACK"
                buttonColor: "#FFFFFF"
                implicitWidth: 110
                implicitHeight: 42
                onClicked: root.goBack()
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 3

                Text {
                    text: "EVERYDAY WORDS"
                    color: "#000000"
                    font.pixelSize: 15
                    font.bold: true
                    font.letterSpacing: 1
                }

                Text {
                    text: "STAGE 01  /  A1 BEGINNER"
                    color: "#737373"
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1
                }
            }

            Item {
                width: parent.width - 110 - 195 - 95 - 120 - 56
                height: 1
            }

            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: "SCORE  1,250"
                color: "#000000"
                font.pixelSize: 12
                font.bold: true
            }

            Rectangle {
                width: 88
                height: 36
                anchors.verticalCenter: parent.verticalCenter
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 2

                Text {
                    anchors.centerIn: parent
                    text: "00:42"
                    color: "#000000"
                    font.pixelSize: 13
                    font.bold: true
                }
            }

            BrutalButton {
                text: "PAUSE"
                buttonColor: "#FFEA2A"
                implicitWidth: 100
                implicitHeight: 42
            }
        }

        RowLayout {
            width: parent.width
            height: parent.height - 70
            spacing: 18

            Rectangle {
                Layout.preferredWidth: 244
                Layout.fillHeight: true
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 2

                Column {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 10

                    Text {
                        text: "QUESTIONS"
                        color: "#000000"
                        font.pixelSize: 12
                        font.bold: true
                        font.letterSpacing: 1
                    }

                    Repeater {
                        model: 10

                        delegate: Rectangle {
                            id: questionDelegate

                            required property int index
                            width: parent.width
                            height: 38
                            color: index < 3 ? "#D1FAE5" : index === 3 ? "#FFEA2A" : "#F4F4F0"
                            border.color: "#000000"
                            border.width: 1

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 10
                                anchors.rightMargin: 8

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: "Question " + (questionDelegate.index + 1)
                                    color: "#000000"
                                    font.pixelSize: 11
                                    font.bold: questionDelegate.index === 3
                                }

                                Item {
                                    width: parent.width - 110
                                    height: 1
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    text: questionDelegate.index < 3 ? "DONE" : questionDelegate.index === 3 ? "NOW" : "..."
                                    color: "#000000"
                                    font.pixelSize: 9
                                    font.bold: true
                                }
                            }
                        }
                    }

                    Rectangle {
                        width: parent.width
                        height: 2
                        color: "#000000"
                    }

                    Text {
                        text: "SOLVED  3 / 10"
                        color: "#000000"
                        font.pixelSize: 11
                        font.bold: true
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#FFFFFF"
                border.color: "#000000"
                border.width: 2

                Column {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 12

                    Row {
                        width: parent.width
                        height: 28

                        Rectangle {
                            width: 108
                            height: 26
                            color: "#FCE7F3"
                            border.color: "#000000"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: "SCRAMBLE"
                                color: "#000000"
                                font.pixelSize: 10
                                font.bold: true
                            }
                        }

                        Item {
                            width: parent.width - 250
                            height: 1
                        }

                        Text {
                            anchors.verticalCenter: parent.verticalCenter
                            text: "QUESTION 04 OF 10"
                            color: "#000000"
                            font.pixelSize: 10
                            font.bold: true
                        }
                    }

                    Text {
                        width: parent.width
                        horizontalAlignment: Text.AlignHCenter
                        text: "Rearrange the letters to name the picture."
                        color: "#404040"
                        font.pixelSize: 12
                    }

                    Rectangle {
                        width: parent.width
                        height: 270
                        color: "#E5E5E5"
                        border.color: "#000000"
                        border.width: 2

                        Rectangle {
                            anchors.centerIn: parent
                            width: Math.min(parent.width - 40, 500)
                            height: parent.height - 30
                            color: "#DCEBFA"
                            border.color: "#000000"
                            border.width: 1

                            Text {
                                anchors.centerIn: parent
                                text: "IMAGE HINT"
                                color: "#737373"
                                font.pixelSize: 14
                                font.bold: true
                                font.letterSpacing: 2
                            }
                        }
                    }

                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 8

                        Repeater {
                            model: ["S", "C", "H", "O", "O", "L"]

                            delegate: Rectangle {
                                id: tileDelegate

                                required property string modelData
                                width: 48
                                height: 50
                                color: "#FFEA2A"
                                border.color: "#000000"
                                border.width: 2

                                Text {
                                    anchors.centerIn: parent
                                    text: tileDelegate.modelData
                                    color: "#000000"
                                    font.pixelSize: 21
                                    font.bold: true
                                }
                            }
                        }
                    }

                    TextField {
                        id: answerField
                        width: parent.width
                        height: 44
                        placeholderText: "Type your answer here..."
                        font.pixelSize: 13
                        background: Rectangle {
                            color: "#FFFFFF"
                            border.color: "#000000"
                            border.width: 2
                        }
                    }

                    Row {
                        anchors.right: parent.right
                        spacing: 10

                        BrutalButton {
                            text: "HINT"
                            buttonColor: "#FFFFFF"
                            implicitWidth: 90
                            implicitHeight: 38
                        }

                        BrutalButton {
                            text: "CLEAR"
                            buttonColor: "#FFFFFF"
                            implicitWidth: 90
                            implicitHeight: 38
                            onClicked: answerField.clear()
                        }

                        BrutalButton {
                            text: "SUBMIT"
                            buttonColor: "#FFEA2A"
                            implicitWidth: 112
                            implicitHeight: 38
                        }
                    }
                }
            }
        }
    }
}
