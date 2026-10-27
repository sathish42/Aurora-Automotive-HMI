import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    objectName: "DiagnosticsPage"
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18

        SectionTitle {
            title: "Diagnostics"
            subtitle: "Demo signal validity and threshold states — not vehicle diagnostics"
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 112
            radius: 14
            color: Theme.surface
            RowLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16
                ColumnLayout {
                    Layout.fillWidth: true
                    Text {
                        text: "ACTIVE DATA SOURCE"
                        color: Theme.muted
                        font.pixelSize: 10
                        font.weight: Font.Bold
                        font.letterSpacing: 1
                    }
                    Text {
                        text: Vehicle.telemetrySource
                        color: Theme.foreground
                        font.pixelSize: 18
                        font.weight: Font.DemiBold
                    }
                    Text {
                        text: "Values are generated locally; no vehicle bus is connected."
                        color: Theme.muted
                        font.pixelSize: 11
                    }
                }
                Rectangle {
                    radius: 10
                    color: Theme.elevated
                    implicitWidth: validityLabel.implicitWidth + 26
                    implicitHeight: 38
                    Text {
                        id: validityLabel
                        anchors.centerIn: parent
                        text: Vehicle.telemetrySource === "CAN frame simulator"
                              ? (CanDiagnostics.stale ? "○  DATA STALE"
                                 : CanDiagnostics.dataValid ? "●  FRAME VALID"
                                 : CanDiagnostics.acceptedFrames > 0 ? "○  FRAME INVALID"
                                 : "○  WAITING FOR FRAME")
                              : "●  SIMULATOR ACTIVE"
                        color: Vehicle.telemetrySource === "CAN frame simulator" && !CanDiagnostics.dataValid
                               ? Theme.warning : Theme.accent
                        font.pixelSize: 10
                        font.weight: Font.Bold
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 104
            spacing: 12
            MetricCard {
                Layout.fillWidth: true
                label: "Accepted frames"
                value: CanDiagnostics.acceptedFrames.toString()
                detail: "Local simulator frames"
                glyph: "↘"
            }
            MetricCard {
                Layout.fillWidth: true
                label: "Rejected frames"
                value: CanDiagnostics.rejectedFrames.toString()
                detail: "Invalid ID, size, or signal range"
                glyph: "⚠"
                highlight: CanDiagnostics.rejectedFrames > 0 ? Theme.warning : Theme.accent
            }
            MetricCard {
                Layout.fillWidth: true
                label: "Last frame"
                value: CanDiagnostics.dataAgeMs < 0
                       ? "—" : (CanDiagnostics.dataAgeMs / 1000).toFixed(1) + " s"
                detail: CanDiagnostics.stale ? "Stale after 1.0 s" : "Age of latest valid frame"
                glyph: "◷"
                highlight: CanDiagnostics.stale ? Theme.warning : Theme.accent
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surface
            radius: 16
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12
                Text {
                    text: "CURRENT SIMULATED FAULTS"
                    color: Theme.muted
                    font.pixelSize: 10
                    font.weight: Font.Bold
                    font.letterSpacing: 1
                }
                Text {
                    visible: Vehicle.warnings.length === 0
                    text: "No threshold faults in the current demo values."
                    color: Theme.accent
                    font.pixelSize: 13
                }
                Repeater {
                    model: Vehicle.warnings
                    delegate: Rectangle {
                        required property string modelData
                        Layout.fillWidth: true
                        Layout.preferredHeight: 54
                        radius: 10
                        color: Theme.elevated
                        RowLayout {
                            anchors.fill: parent
                            anchors.leftMargin: 16
                            anchors.rightMargin: 16
                            Text { text: "⚠"; color: Theme.warning; font.pixelSize: 18 }
                            Text {
                                Layout.fillWidth: true
                                text: modelData
                                color: Theme.foreground
                                font.pixelSize: 13
                            }
                            Text {
                                text: "DEMO THRESHOLD"
                                color: Theme.warning
                                font.pixelSize: 9
                                font.weight: Font.Bold
                            }
                        }
                    }
                }
                Item { Layout.fillHeight: true }
                Text {
                    Layout.fillWidth: true
                    text: Vehicle.telemetrySource === "CAN frame simulator"
                          ? CanDiagnostics.status
                          : "Internal simulator active. CAN adapter counters are shown for inspection only."
                    color: Theme.muted
                    font.pixelSize: 11
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
