// QSimpleScada SCADA 扩展图元:饼图(占比)
// 接收 value(任意范围),内部归一化到 0~1
//
// 设计:每次 update(value) 把 [minValue, maxValue] 区间的值归一化到 [0,1],
// 显示为高亮扇区占比;其余 3 个扇区等分剩余比例。
//
// 示例:controller->updateValue(ip, boardId, id, 75)
//       → 若 minValue=0, maxValue=100, 则归一化为 0.75,高亮扇区占 75%

import QtQuick 2.0

Rectangle {
    id: root
    color: bgColor
    radius: 6
    border.color: "#334155"
    border.width: 1

    // ===== SCADA 接口 =====
    property var metaData: ["value", "title", "unit",
                           "minValue", "maxValue",
                           "decimals",
                           "bgColor", "highlightColor",
                           "color1", "color2", "color3", "color4"]
    function update(value) {
        var n = Number(value);
        if (isNaN(n)) return;
        // 归一化:[minValue, maxValue] → [0, 1]
        var range = maxValue - minValue;
        var normalized = (range > 0.0001) ? (n - minValue) / range : 0;
        highlightFraction = Math.max(0, Math.min(1, normalized));
        // 其余 3 个扇区等分剩余
        var rest = (1.0 - highlightFraction) / 3.0;
        other1 = rest; other2 = rest; other3 = rest;
        // 状态文本
        statusText = root.title + ": " + (highlightFraction * 100).toFixed(decimals) + "%";
        pieCanvas.requestPaint();
    }
    // ===================

    // ===== 可配置属性 =====
    property real value: 0
    property string title: "占比"
    property string unit: "%"
    property real minValue: 0
    property real maxValue: 100
    property int decimals: 1
    property color bgColor: "#0f172a"
    property color highlightColor: "#22d3ee"
    property color color1: "#22d3ee"   // 高亮
    property color color2: "#64748b"   // 其他
    property color color3: "#475569"
    property color color4: "#334155"

    // ===== 内部状态 =====
    property real highlightFraction: 0
    property real other1: 0.33
    property real other2: 0.33
    property real other3: 0.34
    property string statusText: ""

    Column {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 4

        Text {
            text: root.statusText
            color: "#94a3b8"
            font.pixelSize: 11
            font.bold: true
        }

        Canvas {
            id: pieCanvas
            width: Math.min(parent.width, parent.height - captionHeight())
            height: width
            anchors.horizontalCenter: parent.horizontalCenter
            antialiasing: true

            function captionHeight() { return 18; }

            onPaint: {
                var ctx = getContext("2d");
                var cx = width / 2;
                var cy = height / 2;
                var r = Math.min(cx, cy) - 4;

                ctx.clearRect(0, 0, width, height);

                var start = -Math.PI / 2;   // 从 12 点钟方向开始
                var segs = [
                    { value: root.highlightFraction, color: root.color1 },
                    { value: root.other1,           color: root.color2 },
                    { value: root.other2,           color: root.color3 },
                    { value: root.other3,           color: root.color4 }
                ];

                for (var i = 0; i < segs.length; ++i) {
                    var sweep = segs[i].value * 2 * Math.PI;
                    if (sweep <= 0) continue;
                    ctx.beginPath();
                    ctx.moveTo(cx, cy);
                    ctx.arc(cx, cy, r, start, start + sweep);
                    ctx.closePath();
                    ctx.fillStyle = segs[i].color;
                    ctx.fill();
                    start += sweep;
                }

                // 中心挖空(环形效果)
                ctx.beginPath();
                ctx.arc(cx, cy, r * 0.55, 0, 2 * Math.PI);
                ctx.fillStyle = root.bgColor;
                ctx.fill();

                // 中心文字
                ctx.fillStyle = "#e2e8f0";
                ctx.font = "bold " + Math.max(14, r * 0.32) + "px Consolas";
                ctx.textAlign = "center";
                ctx.textBaseline = "middle";
                ctx.fillText((root.highlightFraction * 100).toFixed(root.decimals) + "%", cx, cy);
            }
        }
    }
}
