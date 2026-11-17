// ResultScreen.qml – summary screen
import QtQuick 2.15
import QtQuick.Controls 2.15
import WordSprint 1.0

Item {
    width: 400; height: 300
    Column {
        anchors.centerIn: parent
        spacing: 20

        Text {
            text: wordVM.won ? "You Win!" : "You Lose!"
            font.pixelSize: 32
            color: wordVM.won ? "green" : "red"
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Text {
            text: "Score: " + wordVM.score
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        Row {
            spacing: 10
            Button {
                text: "Play Again"
                onClicked: {
                    wordVM.resetGame()
                    // Clear stack then go to menu
                    rootStack.pop(rootStack.depth)
                    rootStack.push("MenuScreen.qml")
                }
            }
            Button {
                text: "Back to Menu"
                onClicked: {
                    wordVM.resetGame()
                    rootStack.pop(rootStack.depth)
                }
            }
        }
    }
}
