import QtQuick
import Qt.labs.lottieqt

Item {
    id: root

    property string animationType: "correct"
    property bool active: false
    property int loops: 1

    readonly property url animationSource: {
        if (!active)
            return "";

        switch (animationType) {
        case "correct":
            return "qrc:/lottie/result-correct.json";
        case "incorrect":
            return "qrc:/lottie/result-incorrect.json";
        case "summary":
            return "qrc:/lottie/summary-complete.json";
        default:
            return "";
        }
    }

    signal finished

    function play() {
        if (active && animation.status === LottieAnimation.Ready)
            animation.gotoAndPlay(0);
    }

    function stop() {
        if (animation.status === LottieAnimation.Ready)
            animation.stop();
    }

    function replay() {
        play();
    }

    onActiveChanged: {
        if (active)
            play();
        else
            stop();
    }

    LottieAnimation {
        id: animation
        anchors.fill: parent
        source: root.animationSource
        autoPlay: false
        loops: root.loops
        onStatusChanged: {
            if (status === LottieAnimation.Ready && root.active)
                root.play();
            else if (status === LottieAnimation.Error)
                root.active = false;
        }
        onFinished: root.finished()
    }

    visible: active
}
