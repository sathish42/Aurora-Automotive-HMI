import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    id: page
    property real setPointC: 21
    property bool climateOn: true
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle { title: "Climate"; subtitle: "Cabin comfort controls  •  Demo controls do not operate a vehicle" }
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
                    spacing: 18
                    Text { text: "CABIN SET POINT"; color: Theme.muted; font.pixelSize: 11; font.letterSpacing: 1; Layout.alignment: Qt.AlignHCenter }
                    RowLayout {
                        spacing: 28
                        Button {
                            text: "−"
                            onClicked: page.setPointC = Math.max(16, page.setPointC - (Settings.metricUnits ? 1 : 5 / 9))
                        }
                        Text {
                            text: Math.round(Settings.metricUnits ? page.setPointC : page.setPointC * 9 / 5 + 32)
                                  + (Settings.metricUnits ? "°C" : "°F")
                            color: Theme.foreground
                            font.pixelSize: 54
                            font.weight: Font.Light
                        }
                        Button {
                            text: "+"
                            onClicked: page.setPointC = Math.min(30, page.setPointC + (Settings.metricUnits ? 1 : 5 / 9))
                        }
                    }
                    Text {
                        text: "Current cabin "
                              + (Settings.metricUnits ? Vehicle.temperatureC : Vehicle.temperatureC * 9 / 5 + 32).toFixed(1)
                              + (Settings.metricUnits ? " °C" : " °F")
                        color: Theme.muted
                        font.pixelSize: 12
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Switch { text: climateOn ? "Climate on" : "Climate off"; checked: climateOn; onToggled: climateOn = checked; Layout.alignment: Qt.AlignHCenter }
                }
            }
            ColumnLayout {
                Layout.preferredWidth: 280
                Layout.fillHeight: true
                spacing: 14
                MetricCard { Layout.fillWidth: true; label: "Fan"; value: "Auto  •  Level 2"; detail: "Airflow"; glyph: "≋" }
                MetricCard { Layout.fillWidth: true; label: "Air distribution"; value: "Face + feet"; detail: "Vent mode"; glyph: "↗" }
                MetricCard { Layout.fillWidth: true; label: "Air quality"; value: "Good"; detail: "Illustrative status"; glyph: "✧" }
                Button { Layout.fillWidth: true; text: "AUTO"; highlighted: true }
                Button { Layout.fillWidth: true; text: "Defrost"; }
            }
        }
    }
}
