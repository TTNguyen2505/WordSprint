import QtQuick 2.15
import QtQuick.Controls 2.15
import WordSprint 1.0

ApplicationWindow {
    id: mainWindow
    width: 900
    height: 650
    visible: true
    title: "WordSprint"

    StackView {
        id: rootStack
        anchors.fill: parent
        initialItem: MenuScreen {}
    }
}
