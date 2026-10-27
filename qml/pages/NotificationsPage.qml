import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle { title: "Notification center"; subtitle: "Vehicle and system messages" }
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: Theme.surface
            radius: 18
            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 12
                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "RECENT"; color: Theme.muted; font.pixelSize: 10; font.weight: Font.Bold; font.letterSpacing: 1; Layout.fillWidth: true }
                    Text { text: Settings.notificationsEnabled ? "Notifications enabled" : "Notifications muted"; color: Theme.accent; font.pixelSize: 11 }
                }
                Repeater {
                    model: Settings.notificationsEnabled ? Vehicle.warnings : []
                    delegate: Rectangle {
                        required property string modelData
                        Layout.fillWidth: true
                        Layout.preferredHeight: 74
                        color: Theme.elevated
                        radius: 12
                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 15
                            Text { text: "⚠"; color: Theme.warning; font.pixelSize: 22 }
                            ColumnLayout {
                                Layout.fillWidth: true
                                Text { text: modelData; color: Theme.foreground; font.pixelSize: 13; font.weight: Font.DemiBold }
                                Text { text: "Live simulated vehicle status"; color: Theme.muted; font.pixelSize: 11 }
                            }
                            Text { text: "NOW"; color: Theme.warning; font.pixelSize: 10; font.weight: Font.Bold }
                        }
                    }
                }
                Rectangle {
                    visible: Settings.notificationsEnabled
                    Layout.fillWidth: true
                    Layout.preferredHeight: 74
                    color: Theme.elevated
                    radius: 12
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 15
                        Text { text: "●"; color: Theme.accent; font.pixelSize: 18 }
                        ColumnLayout {
                            Layout.fillWidth: true
                            Text { text: "Vehicle systems ready"; color: Theme.foreground; font.pixelSize: 13; font.weight: Font.DemiBold }
                            Text { text: "Simulation started successfully"; color: Theme.muted; font.pixelSize: 11 }
                        }
                        Text { text: "SYSTEM"; color: Theme.muted; font.pixelSize: 10; font.weight: Font.Bold }
                    }
                }
                Rectangle {
                    visible: !Settings.notificationsEnabled
                    Layout.fillWidth: true
                    Layout.preferredHeight: 74
                    color: Theme.elevated
                    radius: 12
                    Text {
                        anchors.centerIn: parent
                        text: "Notifications are disabled in Settings."
                        color: Theme.muted
                        font.pixelSize: 12
                    }
                }
                Item { Layout.fillHeight: true }
                Text {
                    text: "Notifications are illustrative and are not diagnostic warnings."
                    color: Theme.muted
                    font.pixelSize: 11
                }
            }
        }
    }
}
