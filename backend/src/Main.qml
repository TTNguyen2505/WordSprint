import QtQuick
import QtQuick.Window
import QtWebEngine
import QtWebChannel

Window {
    id: root
    maximumWidth: 1200
    maximumHeight: 900
    minimumWidth: 1200
    minimumHeight: 900
    visible: true
    title: qsTr("WordSprint")

    property url initialUrl: "http://localhost:5173"
    required property QtObject bridgeController
    property bool channelReady: false

    WebChannel {
        id: channel
    }

    Component.onCompleted: {
        channel.registerObject("bridgeController", root.bridgeController)
        root.channelReady = true
    }

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: root.channelReady ? root.initialUrl : ""
        webChannel: channel

        settings.accelerated2dCanvasEnabled: true
        settings.webGLEnabled: true
        settings.localContentCanAccessRemoteUrls: true
        settings.localContentCanAccessFileUrls: true
        settings.javascriptCanAccessClipboard: true
    }
}
