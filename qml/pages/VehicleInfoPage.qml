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
            title: "Vehicle information"
            subtitle: "System status and live simulated sensor readings"
        }
        GridLayout {
            Layout.fillWidth: true
            columns: 3
            rowSpacing: 14
            columnSpacing: 14
            MetricCard {
                Layout.fillWidth: true
                label: "Odometer"
                value: (Settings.metricUnits ? Vehicle.odometerKm : Vehicle.odometerKm * 0.621371).toFixed(1)
                       + (Settings.metricUnits ? " km" : " mi")
                detail: "Lifetime distance"
                glyph: "◷"
            }
            MetricCard { Layout.fillWidth: true; label: "Fuel level"; value: Vehicle.fuelPercent.toFixed(1) + "%"; detail: "Simulated tank level"; glyph: "◒"; highlight: Theme.warning }
            MetricCard { Layout.fillWidth: true; label: "Battery SOC"; value: Vehicle.batterySoc.toFixed(1) + "%"; detail: "High voltage"; glyph: "ϟ" }
            MetricCard {
                Layout.fillWidth: true
                label: "Cabin temperature"
                value: (Settings.metricUnits ? Vehicle.temperatureC : Vehicle.temperatureC * 9 / 5 + 32).toFixed(1)
                       + (Settings.metricUnits ? " °C" : " °F")
                detail: "Interior"
                glyph: "☼"
            }
            MetricCard {
                Layout.fillWidth: true; label: "Front left tire"
                value: (Settings.metricUnits ? Vehicle.tirePressureFrontLeft : Vehicle.tirePressureFrontLeft * 14.5038).toFixed(1)
                       + (Settings.metricUnits ? " bar" : " psi")
                detail: "Pressure"; glyph: "◉"
            }
            MetricCard {
                Layout.fillWidth: true; label: "Front right tire"
                value: (Settings.metricUnits ? Vehicle.tirePressureFrontRight : Vehicle.tirePressureFrontRight * 14.5038).toFixed(1)
                       + (Settings.metricUnits ? " bar" : " psi")
                detail: "Pressure"; glyph: "◉"
            }
            MetricCard {
                Layout.fillWidth: true; label: "Rear left tire"
                value: (Settings.metricUnits ? Vehicle.tirePressureRearLeft : Vehicle.tirePressureRearLeft * 14.5038).toFixed(1)
                       + (Settings.metricUnits ? " bar" : " psi")
                detail: "Pressure"; glyph: "◉"
            }
            MetricCard {
                Layout.fillWidth: true; label: "Rear right tire"
                value: (Settings.metricUnits ? Vehicle.tirePressureRearRight : Vehicle.tirePressureRearRight * 14.5038).toFixed(1)
                       + (Settings.metricUnits ? " bar" : " psi")
                detail: "Pressure"; glyph: "◉"
            }
            MetricCard { Layout.fillWidth: true; label: "Powertrain"; value: Vehicle.rpm + " rpm"; detail: "Current engine speed"; glyph: "⚙" }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 58
            radius: 12
            color: Theme.elevated
            RowLayout {
                anchors.fill: parent
                anchors.margins: 16
                Text { text: "DATA SOURCE"; color: Theme.muted; font.pixelSize: 10; font.weight: Font.DemiBold }
                Text { text: "Local deterministic simulator"; color: Theme.foreground; font.pixelSize: 12; Layout.fillWidth: true }
                Text { text: "NOT CONNECTED TO VEHICLE"; color: Theme.warning; font.pixelSize: 10; font.weight: Font.Bold }
            }
        }
    }
}
