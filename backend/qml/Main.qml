import QtQuick
import QtQuick.Controls

ApplicationWindow {
    id: root
    width: 1200
    height: 900
    minimumWidth: 1024
    minimumHeight: 720
    visible: true
    title: qsTr("WordSprint")

    StackView {
        anchors.fill: parent
        initialItem: MenuScreen {}
    }
}
