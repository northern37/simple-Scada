// 侧边导航栏(借鉴 QDashBoard 的 Sidebar 风格)
// SCADA 多视图切换:实时监控 / 历史趋势 / 告警 / 设置

import QtQuick 2.0

Rectangle {
    id: root
    width: 200
    color: "#1e293b"

    property int currentIndex: 0
    signal itemClicked(int index)

    Column {
        anchors.fill: parent
        anchors.topMargin: 8
        spacing: 2

        Repeater {
            model: [
                { icon: "📊", text: "实时监控" },
                { icon: "📈", text: "历史趋势" },
                { icon: "⚠️",  text: "告警事件" },
                { icon: "⚙️", text: "系统设置" }
            ]

            delegate: Rectangle {
                width: root.width
                height: 40
                color: index === root.currentIndex ? "#334155" : "transparent"

                Row {
                    anchors.fill: parent
                    anchors.leftMargin: 16
                    spacing: 12

                    Text {
                        text: modelData.icon
                        font.pixelSize: 16
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    Text {
                        text: modelData.text
                        color: index === root.currentIndex ? "#22d3ee" : "#cbd5e1"
                        font.pixelSize: 13
                        font.bold: index === root.currentIndex
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.itemClicked(index)
                }
            }
        }
    }

    // 右侧分隔线
    Rectangle {
        width: 1
        height: parent.height
        color: "#334155"
        anchors.right: parent.right
    }
}
