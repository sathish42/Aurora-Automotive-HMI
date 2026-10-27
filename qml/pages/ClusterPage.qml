import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    objectName: "ClusterPage"
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: parent.width < 900 || parent.height < 540 ? 16 : 28
        spacing: parent.width < 900 || parent.height < 540 ? 10 : 18
        SectionTitle {
            title: "Digital instrument cluster"
            subtitle: "Driver-focused essentials  •  Demo visualization only"
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 175
            color: "#050b14"
            radius: 24
            RowLayout {
                anchors.fill: parent
                anchors.margins: 42
                spacing: 24
                Gauge {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    value: Vehicle.rpm
                    maximum: 7000
                    unit: "rpm"
                    caption: "ENGINE"
                    color: Theme.warning
                }
                ColumnLayout {
                    Layout.preferredWidth: 210
                    Layout.alignment: Qt.AlignVCenter
                    spacing: 12
                    Text {
                        text: (Vehicle.leftTurnSignal ? "◀" : " ") + "      "
                              + (Vehicle.rightTurnSignal ? "▶" : " ")
                        color: Theme.accent
                        font.pixelSize: 20
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Text {
                        text: Math.round(Settings.metricUnits ? Vehicle.speedKph : Vehicle.speedKph * 0.621371)
                        color: Theme.foreground
                        font.pixelSize: 76
                        font.weight: Font.Light
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Text {
                        text: Settings.metricUnits ? "km/h" : "mph"
                        color: Theme.muted
                        font.pixelSize: 17
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Rectangle {
                        Layout.alignment: Qt.AlignHCenter
                        color: Theme.elevated
                        radius: 10
                        implicitWidth: 88
                        implicitHeight: 42
                        Text {
                            anchors.centerIn: parent
                            text: "GEAR  " + Vehicle.gear
                            color: Theme.foreground
                            font.pixelSize: 14
                            font.weight: Font.DemiBold
                        }
                    }
                }
                Gauge {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    value: Vehicle.fuelPercent
                    maximum: 100
                    unit: "%"
                    caption: "FUEL"
                    color: Theme.accent
                }
            }
        }
        RowLayout {
            Layout.fillWidth: true
            MetricCard { Layout.fillWidth: true; label: "Drive mode"; value: Vehicle.driveMode; glyph: "◇" }
            MetricCard { Layout.fillWidth: true; label: "Battery"; value: Math.round(Vehicle.batterySoc) + "%"; glyph: "ϟ" }
            MetricCard {
                Layout.fillWidth: true
                label: "Odometer"
                value: Math.round(Settings.metricUnits ? Vehicle.odometerKm : Vehicle.odometerKm * 0.621371)
                       + (Settings.metricUnits ? " km" : " mi")
                glyph: "◷"
            }
        }
        Text {
            text: "Prototype display — not validated for road use or safety-critical decisions."
            color: Theme.warning
            font.pixelSize: 11
        }
    }
}
