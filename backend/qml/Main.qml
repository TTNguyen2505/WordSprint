import QtQuick
import QtQuick.Window
import QtWebChannel
import QtWebEngine

Window {
    id: root
    maximumWidth: 1200
    maximumHeight: 900
    minimumWidth: 1200
    minimumHeight: 900
    visible: true
    title: qsTr("WordSprint")

    property url initialUrl: "http://localhost:5173"

    WebChannel {
        id: webChannel
        registeredObjects: [startupManager]
    }

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: root.initialUrl
        webChannel: webChannel

        settings.accelerated2dCanvasEnabled: true
        settings.webGLEnabled: true
        settings.localContentCanAccessRemoteUrls: true
        settings.localContentCanAccessFileUrls: true
        settings.javascriptCanAccessClipboard: true
    }
}
