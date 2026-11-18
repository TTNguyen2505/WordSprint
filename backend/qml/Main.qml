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

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: root.initialUrl

        // Attach the QWebChannel instance exposed from main.cpp so that
        // the React frontend can access the "bridge" object via qwebchannel.js.
        webChannel: webChannel

        settings.accelerated2dCanvasEnabled: true
        settings.webGLEnabled: true
        settings.localContentCanAccessRemoteUrls: true
        settings.localContentCanAccessFileUrls: true
        settings.javascriptCanAccessClipboard: true
    }
}
