import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle {
            title: "Navigation"
            subtitle: "Route guidance concept screen — offline demonstration"
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            radius: 18
            clip: true
            gradient: Gradient {
                GradientStop { position: 0; color: "#13263a" }
                GradientStop { position: 1; color: "#0b1726" }
            }
            Canvas {
                anchors.fill: parent
                onPaint: {
                    const ctx = getContext("2d")
                    ctx.clearRect(0, 0, width, height)
                    ctx.lineWidth = 2
                    ctx.strokeStyle = "#29425a"
                    for (let i = 0; i < 10; ++i) {
                        ctx.beginPath()
                        ctx.moveTo(width * (i / 9), 0)
                        ctx.lineTo(width * (i / 9 + 0.12), height)
                        ctx.stroke()
                    }
                    for (let j = 0; j < 8; ++j) {
                        ctx.beginPath()
                        ctx.moveTo(0, height * (j / 7))
                        ctx.lineTo(width, height * (j / 7 + 0.07))
                        ctx.stroke()
                    }
                    ctx.lineWidth = 8
                    ctx.lineCap = "round"
                    ctx.strokeStyle = "#42d6c5"
                    ctx.beginPath()
                    ctx.moveTo(width * 0.17, height * 0.78)
                    ctx.bezierCurveTo(width * 0.35, height * 0.69, width * 0.34, height * 0.32, width * 0.53, height * 0.47)
                    ctx.bezierCurveTo(width * 0.67, height * 0.59, width * 0.72, height * 0.25, width * 0.86, height * 0.18)
                    ctx.stroke()
                }
            }
            Rectangle {
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 20
                width: 220
                height: 88
                radius: 14
                color: Theme.surface
                Column {
                    anchors.centerIn: parent
                    spacing: 6
                    Text {
                        text: "↗  " + (Settings.metricUnits ? "2.4 km" : "1.5 mi")
                        color: Theme.accent
                        font.pixelSize: 20
                        font.weight: Font.DemiBold
                    }
                    Text { text: "Continue on Lakeside Road"; color: Theme.muted; font.pixelSize: 11 }
                }
            }
            Rectangle {
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.margins: 20
                width: 160
                height: 80
                radius: 14
                color: Theme.surface
                Column {
                    anchors.centerIn: parent
                    spacing: 6
                    Text { text: "ARRIVAL"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 1 }
                    Text { text: "18:42  •  12 min"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold }
                }
            }
        }
        Text {
            text: "Map, route, and arrival details are illustrative only; no GNSS or routing service is connected."
            color: Theme.muted
            font.pixelSize: 11
        }
    }
}
