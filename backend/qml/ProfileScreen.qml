import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#858585"

    function openScreen(fileName) {
        // StackView.view is an attached property on the current screen.
        // qmllint disable unqualified
        StackView.view.push(Qt.resolvedUrl(fileName));
        // qmllint enable unqualified
    }

    function goBack() {
        // qmllint disable unqualified
        StackView.view.pop();
        // qmllint enable unqualified
    }

    Rectangle {
        anchors.centerIn: parent
        width: Math.min(parent.width - 48, 560)
        height: 500
        color: "#FFFFFF"
        border.color: "#000000"
        border.width: 2

        Column {
            anchors.fill: parent
            anchors.margins: 28
            spacing: 16

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: "SELECT A PROFILE"
                color: "#000000"
                font.pixelSize: 24
                font.bold: true
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
                    width: parent.width * 0.62
                    anchors.verticalCenter: parent.verticalCenter
                    text: "USERNAME"
                    color: "#404040"
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1
                }

                Text {
                    width: parent.width * 0.22
                    anchors.verticalCenter: parent.verticalCenter
                    text: "SCORE"
                    color: "#404040"
                    font.pixelSize: 10
                    font.bold: true
                    font.letterSpacing: 1
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter
                    text: "SELECT"
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
                            name: "WordSprinter",
                            score: "1,250"
                        },
                        {
                            name: "Player Two",
                            score: "840"
                        },
                        {
                            name: "New Player",
                            score: "0"
                        },
                        {
                            name: "Guest",
                            score: "0"
                        }
                    ]

                    delegate: Rectangle {
                        id: profileDelegate

                        required property var modelData
                        required property int index
                        width: parent.width
                        height: 46
                        color: index === 0 ? "#FFEA2A" : "#FFFFFF"
                        border.color: "#000000"
                        border.width: 1

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: 12
                            anchors.rightMargin: 12

                            Text {
                                width: parent.width * 0.62
                                anchors.verticalCenter: parent.verticalCenter
                                text: profileDelegate.modelData.name
                                color: "#000000"
                                font.pixelSize: 13
                                font.bold: profileDelegate.index === 0
                            }

                            Text {
                                width: parent.width * 0.22
                                anchors.verticalCenter: parent.verticalCenter
                                text: profileDelegate.modelData.score
                                color: "#000000"
                                font.pixelSize: 12
                            }

                            Rectangle {
                                width: 16
                                height: 16
                                anchors.verticalCenter: parent.verticalCenter
                                radius: 8
                                color: "#FFFFFF"
                                border.color: "#000000"
                                border.width: 1

                                Rectangle {
                                    anchors.centerIn: parent
                                    width: 8
                                    height: 8
                                    radius: 4
                                    color: profileDelegate.index === 0 ? "#000000" : "#FFFFFF"
                                }
                            }
                        }
                    }
                }
            }

            Row {
                width: parent.width
                spacing: 12

                BrutalButton {
                    text: "+ ADD PROFILE"
                    buttonColor: "#FFFFFF"
                    implicitWidth: 180
                    implicitHeight: 44
                    onClicked: root.openScreen("CreateProfileScreen.qml")
                }

                Item {
                    width: parent.width - 180 - 12 - 118 - 118 - 24
                    height: 1
                }

                BrutalButton {
                    text: "CANCEL"
                    buttonColor: "#FFFFFF"
                    implicitWidth: 118
                    implicitHeight: 44
                    onClicked: root.goBack()
                }

                BrutalButton {
                    text: "SELECT"
                    buttonColor: "#FFEA2A"
                    implicitWidth: 118
                    implicitHeight: 44
                    onClicked: root.goBack()
                }
            }
        }
    }
}
