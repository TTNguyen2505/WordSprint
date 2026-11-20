import QtQuick

Item {
    id: root

    property string title: ""
    property string subtitle: ""
    property bool showBack: true
    signal backRequested

    implicitHeight: 100

    Text {
        anchors.left: parent.left
        anchors.top: parent.top
        text: "WORDSPRINT"
        color: "#000000"
        font.family: "Arial"
        font.pixelSize: 13
        font.bold: true
        font.letterSpacing: 2
    }

    BrutalButton {
        visible: root.showBack
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        implicitWidth: 120
        implicitHeight: 42
        text: "< BACK"
        buttonColor: "#FFFFFF"
        onClicked: root.backRequested()
    }

    Column {
        anchors.centerIn: parent
        spacing: 5

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.title
            color: "#000000"
            font.family: "Arial"
            font.pixelSize: 25
            font.bold: true
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            visible: root.subtitle.length > 0
            text: root.subtitle
            color: "#737373"
            font.family: "Arial"
            font.pixelSize: 12
            font.letterSpacing: 1
        }
    }

    Rectangle {
        anchors.bottom: parent.bottom
        width: parent.width
        height: 2
        color: "#000000"
    }
}
