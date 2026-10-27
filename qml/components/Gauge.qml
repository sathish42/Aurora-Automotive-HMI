import QtQuick
import Auto.HMI 1.0

Item {
    id: root
    property real value: 0
    property real maximum: 100
    property string unit: ""
    property string caption: ""
    property color color: Theme.accent

    Canvas {
        id: dial
        anchors.fill: parent
        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            const cx = width / 2
            const cy = height / 2
            const radius = Math.min(width, height) * 0.40
            const start = Math.PI * 0.75
            const sweep = Math.PI * 1.5
            const fraction = Math.max(0, Math.min(1, root.maximum > 0 ? root.value / root.maximum : 0))
            ctx.lineWidth = Math.max(8, radius * 0.085)
            ctx.lineCap = "round"
            ctx.beginPath()
            ctx.arc(cx, cy, radius, start, start + sweep, false)
            ctx.strokeStyle = Theme.elevated
            ctx.stroke()
            ctx.beginPath()
            ctx.arc(cx, cy, radius, start, start + sweep * fraction, false)
            ctx.strokeStyle = root.color
            ctx.stroke()
        }
    }
    onValueChanged: dial.requestPaint()
    onMaximumChanged: dial.requestPaint()
    onColorChanged: dial.requestPaint()
    Connections {
        target: Theme
        function onPaletteChanged() { dial.requestPaint() }
    }

    Column {
        anchors.centerIn: parent
        spacing: 4
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: Math.round(root.value).toLocaleString(Qt.locale(), "f", 0)
            color: Theme.foreground
            font.pixelSize: Math.min(root.width, root.height) * 0.20
            font.weight: Font.Light
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.unit
            color: Theme.muted
            font.pixelSize: 13
        }
    }
    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        text: root.caption
        color: Theme.muted
        font.pixelSize: 12
        font.letterSpacing: 1
    }
}
