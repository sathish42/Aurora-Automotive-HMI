import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle { title: "Driver assistance"; subtitle: "ADAS concept visualization  •  No sensor or actuator integration" }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface
                radius: 18
                clip: true
                Canvas {
                    anchors.fill: parent
                    onPaint: {
                        const ctx = getContext("2d")
                        ctx.clearRect(0, 0, width, height)
                        ctx.fillStyle = "#101925"
                        ctx.fillRect(0, 0, width, height)
                        ctx.fillStyle = "#273342"
                        ctx.beginPath()
                        ctx.moveTo(width * 0.34, height)
                        ctx.lineTo(width * 0.45, height * 0.2)
                        ctx.lineTo(width * 0.55, height * 0.2)
                        ctx.lineTo(width * 0.66, height)
                        ctx.closePath()
                        ctx.fill()
                        ctx.setLineDash([12, 12])
                        ctx.strokeStyle = "#dce8f4"
                        ctx.lineWidth = 3
                        ctx.beginPath()
                        ctx.moveTo(width * 0.5, height * 0.9)
                        ctx.lineTo(width * 0.5, height * 0.24)
                        ctx.stroke()
                        ctx.setLineDash([])
                    }
                }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 24
                    width: 76
                    height: 120
                    radius: 18
                    color: Theme.accent
                    opacity: 0.85
                    Text { anchors.centerIn: parent; text: "YOU"; color: "#08111f"; font.pixelSize: 11; font.weight: Font.Bold }
                }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.topMargin: parent.height * 0.27
                    width: 50
                    height: 78
                    radius: 12
                    color: Theme.warning
                    Text { anchors.centerIn: parent; text: "VEH"; color: "#08111f"; font.pixelSize: 9; font.weight: Font.Bold }
                }
            }
            ColumnLayout {
                Layout.preferredWidth: 290
                Layout.fillHeight: true
                spacing: 14
                MetricCard { Layout.fillWidth: true; label: "Following distance"; value: "2.4 s"; detail: "Illustrative estimate"; glyph: "↔" }
                MetricCard { Layout.fillWidth: true; label: "Lane status"; value: "Centered"; detail: "No camera connected"; glyph: "⇵" }
                MetricCard { Layout.fillWidth: true; label: "Forward object"; value: "Detected"; detail: "Virtual test object"; glyph: "◎"; highlight: Theme.warning }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Theme.elevated
                    radius: 12
                    Column {
                        anchors.fill: parent
                        anchors.margins: 16
                        spacing: 9
                        Text { text: "SYSTEM STATE"; color: Theme.muted; font.pixelSize: 10; font.weight: Font.Bold; font.letterSpacing: 1 }
                        Text { text: "DEMO ONLY"; color: Theme.warning; font.pixelSize: 19; font.weight: Font.Bold }
                        Text { width: parent.width; text: "This visualization does not represent actual ADAS sensing, alerts, or control. Never use it to guide driving."; color: Theme.foreground; font.pixelSize: 11; wrapMode: Text.WordWrap }
                    }
                }
            }
        }
    }
}
