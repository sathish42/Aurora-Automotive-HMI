import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    objectName: "DashboardPage"
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 26
        spacing: 18
        SectionTitle {
            title: "Good evening"
            subtitle: "Your vehicle at a glance  •  Live simulated telemetry"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 76
            radius: 14
            color: Theme.surface
            border.color: Theme.elevated
            border.width: 1

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 18
                anchors.rightMargin: 14
                spacing: 14

                ColumnLayout {
                    spacing: 4
                    Text {
                        text: "DEMO SCENARIO"
                        color: Theme.muted
                        font.pixelSize: 10
                        font.weight: Font.Bold
                        font.letterSpacing: 0.8
                    }
                    Text {
                        text: Vehicle.simulationRunning ? "Simulation running" : "Simulation paused"
                        color: Vehicle.simulationRunning ? Theme.accent : Theme.warning
                        font.pixelSize: 11
                        font.weight: Font.DemiBold
                    }
                }

                ComboBox {
                    id: scenarioSelector
                    Layout.preferredWidth: 205
                    model: Vehicle.scenarios
                    currentIndex: Vehicle.scenarios.indexOf(Vehicle.scenario)
                    onActivated: Vehicle.setScenario(currentText)
                    Accessible.name: "Select a vehicle demo scenario"
                }

                ComboBox {
                    Layout.preferredWidth: 190
                    model: Vehicle.dataSources
                    currentIndex: Vehicle.dataSources.indexOf(Vehicle.telemetrySource)
                    onActivated: Vehicle.setTelemetrySource(currentText)
                    Accessible.name: "Select a simulated telemetry source"
                }

                Text {
                    Layout.fillWidth: true
                    text: Vehicle.scenarioDescription
                    color: Theme.muted
                    font.pixelSize: 11
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                }

                Button {
                    text: Vehicle.simulationRunning ? "Ⅱ  Pause" : "▶  Resume"
                    Accessible.name: Vehicle.simulationRunning ? "Pause simulation" : "Resume simulation"
                    onClicked: Vehicle.simulationRunning ? Vehicle.pauseSimulation() : Vehicle.startSimulation()
                }

                Button {
                    text: "↺  Reset"
                    Accessible.name: "Reset and replay the selected scenario"
                    onClicked: Vehicle.resetSimulation()
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 18

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: 360
                color: Theme.surface
                radius: 18
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    spacing: 8
                    RowLayout {
                        Layout.fillWidth: true
                        Text {
                            text: "DRIVE STATUS"
                            color: Theme.muted
                            font.pixelSize: 11
                            font.weight: Font.DemiBold
                            font.letterSpacing: 1
                            Layout.fillWidth: true
                        }
                        Text {
                            text: Vehicle.warnings.length > 0 ? "⚠ " + Vehicle.warnings.length + " ALERT"
                                  : "✓ SYSTEMS NORMAL"
                            color: Vehicle.warnings.length > 0 ? Theme.warning : Theme.accent
                            font.pixelSize: 10
                            font.weight: Font.DemiBold
                        }
                        Text {
                            text: Vehicle.leftTurnSignal ? "◀" : ""
                            color: Theme.accent
                            font.pixelSize: 20
                        }
                        Text {
                            text: Vehicle.rightTurnSignal ? "▶" : ""
                            color: Theme.accent
                            font.pixelSize: 20
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        spacing: 8
                        Gauge {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            Layout.minimumWidth: 180
                            value: Settings.metricUnits ? Vehicle.speedKph : Vehicle.speedKph * 0.621371
                            maximum: Settings.metricUnits ? 240 : 150
                            unit: Settings.metricUnits ? "km/h" : "mph"
                            caption: "SPEED"
                        }
                        ColumnLayout {
                            Layout.alignment: Qt.AlignVCenter
                            spacing: 18
                            Text {
                                text: "GEAR"
                                color: Theme.muted
                                font.pixelSize: 10
                                font.letterSpacing: 1
                            }
                            Text {
                                text: Vehicle.gear
                                color: Theme.foreground
                                font.pixelSize: 38
                                font.weight: Font.Light
                            }
                            Text {
                                text: "MODE"
                                color: Theme.muted
                                font.pixelSize: 10
                                font.letterSpacing: 1
                            }
                            Text {
                                text: Vehicle.driveMode.toUpperCase()
                                color: Theme.accent
                                font.pixelSize: 12
                                font.weight: Font.DemiBold
                            }
                        }
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10
                        Repeater {
                            model: ["Eco", "Comfort", "Sport"]
                            delegate: Button {
                                required property string modelData
                                Layout.fillWidth: true
                                text: modelData
                                highlighted: Vehicle.driveMode === modelData
                                onClicked: Vehicle.setDriveMode(modelData)
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: 255
                Layout.fillHeight: true
                spacing: 14
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: Theme.surface
                    radius: 18
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 18
                        Text {
                            text: "POWERTRAIN"
                            color: Theme.muted
                            font.pixelSize: 11
                            font.weight: Font.DemiBold
                            font.letterSpacing: 1
                        }
                        Gauge {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            value: Vehicle.rpm
                            maximum: 7000
                            unit: "rpm"
                            caption: "ENGINE SPEED"
                            color: Theme.warning
                        }
                    }
                }
                MetricCard {
                    Layout.fillWidth: true
                    label: "Fuel"
                    value: Math.round(Vehicle.fuelPercent) + "%"
                    detail: "Demo range  •  "
                            + Math.round(420 * Vehicle.fuelPercent / 72 * (Settings.metricUnits ? 1 : 0.621371))
                            + (Settings.metricUnits ? " km" : " mi")
                    glyph: "◒"
                    highlight: Theme.warning
                }
            }
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 4
            rowSpacing: 12
            columnSpacing: 12
            MetricCard {
                Layout.fillWidth: true
                label: "Battery"
                value: Math.round(Vehicle.batterySoc) + "%"
                detail: "High voltage state of charge"
                glyph: "ϟ"
            }
            MetricCard {
                Layout.fillWidth: true
                label: "Odometer"
                value: Math.round(Settings.metricUnits ? Vehicle.odometerKm : Vehicle.odometerKm * 0.621371).toLocaleString()
                       + (Settings.metricUnits ? " km" : " mi")
                detail: "Total distance"
                glyph: "◷"
            }
            MetricCard {
                Layout.fillWidth: true
                label: "Cabin"
                value: (Settings.metricUnits ? Vehicle.temperatureC : Vehicle.temperatureC * 9 / 5 + 32).toFixed(1)
                       + (Settings.metricUnits ? " °C" : " °F")
                detail: "Interior temperature"
                glyph: "☼"
            }
            MetricCard {
                id: tirePressureCard
                objectName: "tirePressureCard"
                Layout.fillWidth: true
                label: "Tire pressure"
                value: (Settings.metricUnits ? Vehicle.tirePressureFrontLeft : Vehicle.tirePressureFrontLeft * 14.5038).toFixed(1)
                       + (Settings.metricUnits ? " bar" : " psi")
                detail: Vehicle.tirePressureFrontLeft < 1.9
                        ? "Front-left low-pressure warning"
                        : "All tires in nominal demo range"
                glyph: "◉"
            }
        }
    }
}
