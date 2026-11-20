import QtQuick
import QtQuick.Controls

Rectangle {
    id: root

    color: "#F4F4F0"

    property int count: 3

    function startGame() {
        // StackView.view is an attached property on the current screen.
        // qmllint disable unqualified
        StackView.view.replace(Qt.resolvedUrl("GameScreen.qml"));
        // qmllint enable unqualified
    }

    Timer {
        id: countdownTimer
        interval: 800
        repeat: true
        running: true

        onTriggered: {
            if (root.count > 1) {
                root.count -= 1;
            } else {
                countdownTimer.stop();
                root.startGame();
            }
        }
    }

    Text {
        anchors.centerIn: parent
        text: root.count
        color: "#000000"
        font.pixelSize: 96
        font.bold: true
    }
}
