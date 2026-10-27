import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    property bool playing: false
    property real progress: 0.34
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle { title: "Media"; subtitle: "Audio experience  •  Local demo content" }
        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.surface
                radius: 18
                ColumnLayout {
                    anchors.centerIn: parent
                    spacing: 16
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        width: 210
                        height: 210
                        radius: 24
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#247b85" }
                            GradientStop { position: 1; color: "#182b57" }
                        }
                        Text { anchors.centerIn: parent; text: "♫"; color: "white"; font.pixelSize: 72 }
                    }
                    Text { text: "Midnight Drive"; color: Theme.foreground; font.pixelSize: 23; font.weight: Font.DemiBold; Layout.alignment: Qt.AlignHCenter }
                    Text { text: "Aurora Radio  •  Electronic"; color: Theme.muted; font.pixelSize: 12; Layout.alignment: Qt.AlignHCenter }
                    Slider { Layout.preferredWidth: 380; from: 0; to: 1; value: progress; onMoved: progress = value }
                    RowLayout {
                        Layout.alignment: Qt.AlignHCenter
                        Button { text: "◀"; onClicked: progress = Math.max(0, progress - 0.1) }
                        Button { text: playing ? "Ⅱ" : "▶"; highlighted: true; onClicked: playing = !playing }
                        Button { text: "▶"; onClicked: progress = Math.min(1, progress + 0.1) }
                    }
                }
            }
            ColumnLayout {
                Layout.preferredWidth: 260
                Layout.fillHeight: true
                spacing: 14
                MetricCard { Layout.fillWidth: true; label: "Output"; value: "Aurora Radio"; detail: "Streaming source (mock)"; glyph: "♫" }
                Text { text: "SOUND"; color: Theme.muted; font.pixelSize: 10; font.weight: Font.Bold; font.letterSpacing: 1 }
                Slider { Layout.fillWidth: true; from: 0; to: 100; value: Settings.volume; onMoved: Settings.volume = Math.round(value) }
                Text { text: "Volume  " + Settings.volume + "%"; color: Theme.foreground; font.pixelSize: 12 }
                Button { Layout.fillWidth: true; text: "Audio settings"; onClicked: Navigation.navigate("Settings") }
            }
        }
    }
}
