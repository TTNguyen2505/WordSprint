import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import WordSprint 1.0

Window {
    id: root
    visible: true
    width: 1200
    height: 900
    title: qsTr("WordSprint")

    Column {
        anchors.centerIn: parent
        spacing: 12

        Text { text: "Screen: " + appVM.currentScreen }
        Text { text: "Score: " + gameplayVM.score }
        Text { text: "Time: " + gameplayVM.remainingTime }
        Text { text: "State: " + gameplayVM.gameState }

        Button { text: "Start"; onClicked: gameplayVM.startGame() }
        Button { text: "Pause"; onClicked: gameplayVM.pauseGame() }
        Button { text: "Resume"; onClicked: gameplayVM.resumeGame() }
        Button { text: "Restart"; onClicked: gameplayVM.restartGame() }

        Row {
            spacing: 8
            TextField { id: answerField; placeholderText: "Answer"; width: 200 }
            Button { text: "Submit"; onClicked: gameplayVM.submitAnswer(answerField.text) }
        }
    }
}

