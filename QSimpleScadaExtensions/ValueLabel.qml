// QSimpleScada SCADA 扩展图元:大字号数字标签
// 接口约定(与 EEIoT 一致):
//   - function update(value):SCADA 调用入口,接收数据并刷新
//   - property var metaData:列出可在编辑器中编辑的属性
//
// 示例:controller->updateValue(ip, boardId, id, 25.3)
//       → 此控件的 update(25.3) 被调用 → 显示 "25.30"

import QtQuick 2.0

Rectangle {
    id: root
    color: bgColor
    radius: 6
    border.color: "#334155"
    border.width: 1

    // ===== SCADA 接口 =====
    property var metaData: ["value", "label", "unit",
                           "labelFontSize", "valueFontSize",
                           "bgColor", "labelColor", "valueColor",
                           "decimals"]
    function update(value) {
        // 兼容:SCADA 推过来的可能是 number / string
        var n = Number(value);
        if (isNaN(n)) {
            root.displayText = String(value);
        } else {
            root.displayText = n.toFixed(decimals);
        }
        root.lastValue = n;
    }
    // ===================

    // ===== 可配置属性(默认) =====
    property real value: 0
    property string label: "Sensor"
    property string unit: ""
    property int labelFontSize: 12
    property int valueFontSize: 32
    property int decimals: 2
    property color bgColor: "#0f172a"
    property color labelColor: "#94a3b8"
    property color valueColor: "#22d3ee"

    // ===== 内部状态 =====
    property string displayText: "0.00"
    property real lastValue: 0

    Column {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 2

        Text {
            id: labelText
            text: root.label + (root.unit !== "" ? " (" + root.unit + ")" : "")
            color: root.labelColor
            font.pixelSize: root.labelFontSize
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: valueText
            text: root.displayText
            color: root.valueColor
            font.pixelSize: root.valueFontSize
            font.bold: true
            font.family: "Consolas"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}
