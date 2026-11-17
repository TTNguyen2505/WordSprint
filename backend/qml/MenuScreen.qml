// MenuScreen.qml – entry screen
import QtQuick 2.15
import QtQuick.Controls 2.15
import WordSprint 1.0

Item {
    width: 400; height: 300
    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: "Word Sprint"
            font.pixelSize: 32
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Button {
            text: "Start Game"
            onClicked: {
                // Reset any previous state
                wordVM.resetGame()
                // Navigate to the game screen
                rootStack.push("GameScreen.qml")
            }
        }
    }
}
