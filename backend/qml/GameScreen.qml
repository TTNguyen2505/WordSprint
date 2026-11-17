// GameScreen.qml – playing screen
import QtQuick 2.15
import QtQuick.Controls 2.15
import WordSprint 1.0

Item {
    width: 600; height: 400
    Column {
        anchors.fill: parent
        spacing: 10
        padding: 10

        Text {
            text: "Select words (mock game)"
            font.pixelSize: 24
            horizontalAlignment: Text.AlignHCenter
            width: parent.width
        }

        // Show the word list
        ListView {
            id: wordList
            model: wordVM.words
            delegate: Row {
                spacing: 8
                Text { text: modelData }
                Button {
                    text: "Remove"
                    onClicked: wordVM.removeWord(index)
                }
            }
            height: 200
            clip: true
        }

        Row {
            spacing: 10
            Button {
                text: "Add word"
                onClicked: {
                    // Simple mock – add a random word
                    var w = ["alpha","beta","gamma","delta"][Math.floor(Math.random()*4)]
                    wordVM.addWord(w)
                }
            }
            Button {
                text: "Sort by length"
                onClicked: wordVM.sortByLength()
            }
        }

        // Finish the game – mocked scoring logic
        Button {
            text: "Finish Game"
            onClicked: {
                // Mock: score = number of words * 10, win if > 20
                var sc = wordVM.words.length * 10
                var win = sc > 20
                wordVM.finishGame(sc, win)
                rootStack.push("ResultScreen.qml")
            }
        }
    }
}
