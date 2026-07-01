// QSimpleScada SCADA 扩展图元:实时趋势图
// 显示最近 maxPoints 个数据点的折线图(自动滚动窗口)

import QtQuick 2.0

Rectangle {
    id: root
    color: bgColor
    radius: 6
    border.color: "#334155"
    border.width: 1
    // 让根 Rectangle 强制有大小(防止 QQuickWidget 嵌入时父布局没正确传递尺寸)
    anchors.fill: parent
    width: parent ? parent.width : 360
    height: parent ? parent.height : 360

    // ===== SCADA 接口 =====
    property var metaData: ["value", "label", "unit",
                           "min", "max",
                           "maxPoints",
                           "bgColor", "lineColor", "gridColor",
                           "decimals"]
    function update(value) {
        var n = Number(value);
        if (isNaN(n)) return;

        history.push(n);
        if (history.length > maxPoints) {
            history.shift();
        }

        if (autoScale) {
            var mn = history[0], mx = history[0];
            for (var i = 1; i < history.length; ++i) {
                if (history[i] < mn) mn = history[i];
                if (history[i] > mx) mx = history[i];
            }
            if (mx - mn < 0.01) { mx = mn + 1.0; }
            var pad = (mx - mn) * 0.1;
            min = mn - pad;
            max = mx + pad;
        }

        statusText = root.label + ": " + n.toFixed(decimals) +
                     (root.unit !== "" ? " " + root.unit : "") +
                     "  [N=" + history.length + "]";
        chartCanvas.requestPaint();
    }
    // ===================

    // ===== 可配置属性 =====
    property real value: 0
    property string label: "Trend"
    property string unit: ""
    property real min: -1e9
    property real max:  1e9
    property int maxPoints: 60
    property int decimals: 2
    property bool autoScale: true
    property color bgColor: "#0f172a"
    property color lineColor: "#22d3ee"
    property color gridColor: "#334155"

    // ===== 内部状态 =====
    property var history: []
    property string statusText: "等待数据..."

    Column {
        anchors.fill: parent
        anchors.margins: 4
        spacing: 2

        Text {
            id: caption
            text: root.statusText
            color: "#94a3b8"
            font.pixelSize: 11
            font.bold: true
        }

        Canvas {
            id: chartCanvas
            width: parent.width
            height: Math.max(50, parent.height - caption.height - 4)
            antialiasing: true
            renderTarget: Canvas.Image

            onWidthChanged: requestPaint()
            onHeightChanged: requestPaint()

            onPaint: {
                var ctx = getContext("2d");

                // ★ 调试:画可见边框,确认 Canvas 大小
                ctx.clearRect(0, 0, width, height);
                ctx.strokeStyle = "#475569";
                ctx.lineWidth = 1;
                ctx.strokeRect(0.5, 0.5, width - 1, height - 1);

                if (root.history.length < 2) {
                    ctx.fillStyle = "#64748b";
                    ctx.font = "12px Consolas";
                    ctx.textAlign = "center";
                    ctx.fillText("等待数据 (N=" + root.history.length + ")",
                                 width / 2, height / 2);
                    return;
                }

                // 背景网格
                ctx.strokeStyle = root.gridColor;
                ctx.lineWidth = 1;
                for (var g = 0; g <= 4; ++g) {
                    var y = (height * g) / 4;
                    ctx.beginPath();
                    ctx.moveTo(0, y);
                    ctx.lineTo(width, y);
                    ctx.stroke();
                }

                // Y 范围
                var yMin = root.min;
                var yMax = root.max;
                if (yMax - yMin < 0.001) yMax = yMin + 1;

                // 折线
                ctx.strokeStyle = root.lineColor;
                ctx.lineWidth = 2;
                ctx.beginPath();
                var n = root.history.length;
                for (var i = 0; i < n; ++i) {
                    var x = (i / Math.max(1, root.maxPoints - 1)) * width;
                    var yNorm = (root.history[i] - yMin) / (yMax - yMin);
                    var y = height - yNorm * height;
                    if (i === 0) ctx.moveTo(x, y);
                    else ctx.lineTo(x, y);
                }
                ctx.stroke();

                // 最新点高亮
                var lastY = height - ((root.history[n - 1] - yMin) / (yMax - yMin)) * height;
                ctx.fillStyle = root.lineColor;
                ctx.beginPath();
                ctx.arc(width - 3, lastY, 3, 0, 2 * Math.PI);
                ctx.fill();
            }
        }
    }
}
