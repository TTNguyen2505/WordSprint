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
    property QtObject webChannel: null

    WebEngineView {
        id: webEngineView
        anchors.fill: parent
        url: root.initialUrl
        webChannel: root.webChannel

        onJavaScriptConsoleMessage: function (level, message, lineNumber, sourceID) {
            const prefix = "WordSprint.QtQuickAnimation:";
            if (!message.startsWith(prefix)) {
                console.log(message);
                return;
            }

            const event = message.substring(prefix.length);
            if (event === "hide") {
                resultFeedback.active = false;
                return;
            }

            if (event === "correct" || event === "incorrect" || event === "summary") {
                resultFeedback.active = false;
                resultFeedback.animationType = event;
                resultFeedback.active = true;
            }
        }

        settings.accelerated2dCanvasEnabled: true
        settings.webGLEnabled: true
        settings.localContentCanAccessRemoteUrls: true
        settings.localContentCanAccessFileUrls: true
        settings.javascriptCanAccessClipboard: true
    }

    ResultLottieFeedback {
        id: resultFeedback
        anchors.centerIn: parent
        width: 144
        height: 144
        z: 1
        active: false
        enabled: false
    }
}
