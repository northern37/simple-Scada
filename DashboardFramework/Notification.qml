// 通知组件(借鉴 QDashBoard 的 Notification.qml 风格)
// 右下角浮窗,自动消失

import QtQuick 2.0

Rectangle {
    id: root
    width: 320
    height: 72
    radius: 8
    color: bgColor
    border.color: borderColor
    border.width: 1

    // 调用入口:show(level, title, message)
    function show(level, title, message) {
        root.tLevel = level;
        root.tTitle = title;
        root.tMessage = message;
        root.opacity = 1.0;
        root.visible = true;
        hideTimer.restart();
    }

    property string tLevel: "info"
    property string tTitle: ""
    property string tMessage: ""

    property color bgColor: tLevel === "error" ? "#7f1d1d" :
                           tLevel === "warn"  ? "#78350f" :
                                                "#1e3a8a"
    property color borderColor: tLevel === "error" ? "#fca5a5" :
                                tLevel === "warn"  ? "#fcd34d" :
                                                     "#93c5fd"

    // 图标 + 文字
    Row {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Text {
            text: root.tLevel === "error" ? "❌" :
                  root.tLevel === "warn"  ? "⚠️" : "ℹ️"
            font.pixelSize: 24
            anchors.verticalCenter: parent.verticalCenter
        }

        Column {
            anchors.verticalCenter: parent.verticalCenter
            spacing: 4

            Text {
                text: root.tTitle
                color: "#ffffff"
                font.pixelSize: 13
                font.bold: true
            }
            Text {
                text: root.tMessage
                color: "#e2e8f0"
                font.pixelSize: 12
                wrapMode: Text.Wrap
            }
        }
    }

    Behavior on opacity { NumberAnimation { duration: 400 } }
    opacity: 0
    visible: false

    Timer {
        id: hideTimer
        interval: 4000
        repeat: false
        onTriggered: {
            root.opacity = 0;
            hideAnimTimer.restart();
        }
    }
    Timer {
        id: hideAnimTimer
        interval: 500
        repeat: false
        onTriggered: root.visible = false
    }
}
