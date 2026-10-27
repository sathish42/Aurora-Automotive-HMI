import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Auto.HMI 1.0

Item {
    id: page
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 28
        spacing: 18
        SectionTitle { title: "Settings"; subtitle: "Personalize this demonstration interface" }
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 12
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: page.height < 720 ? 156 : 172
                color: Theme.surface
                radius: 14
                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    height: 56
                    anchors.margins: 20
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "Appearance"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: Settings.darkTheme ? "Dark cockpit theme" : "Light theme"; color: Theme.muted; font.pixelSize: 11 }
                    }
                    Switch {
                        checked: Settings.darkTheme
                        onToggled: {
                            Settings.darkTheme = checked
                            Theme.dark = checked
                        }
                    }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: page.height < 720 ? 78 : 94
                color: Theme.surface
                radius: 14
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "Units"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: Settings.metricUnits ? "Metric (km, km/h)" : "Imperial (mi, mph)"; color: Theme.muted; font.pixelSize: 11 }
                    }
                    Switch { checked: Settings.metricUnits; onToggled: Settings.metricUnits = checked }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: page.height < 720 ? 78 : 94
                color: Theme.surface
                radius: 14
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "Notifications"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: "Show vehicle and system notifications"; color: Theme.muted; font.pixelSize: 11 }
                    }
                    Switch { checked: Settings.notificationsEnabled; onToggled: Settings.notificationsEnabled = checked }
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: page.height < 720 ? 78 : 94
                color: Theme.surface
                radius: 14
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    ColumnLayout {
                        Layout.fillWidth: true
                        Text { text: "Secondary cluster window"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold }
                        Text { text: "Choose an available display for the desktop preview"; color: Theme.muted; font.pixelSize: 11 }
                    }
                    Switch {
                        checked: Settings.clusterWindowEnabled
                        enabled: Qt.platform.os !== "android"
                        onToggled: Settings.clusterWindowEnabled = checked
                    }
                }
                RowLayout {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.bottomMargin: 43
                    Text {
                        Layout.fillWidth: true
                        text: "Simulate two display targets on this screen"
                        color: Theme.muted
                        font.pixelSize: 11
                        wrapMode: Text.WordWrap
                    }
                    Switch {
                        checked: DisplayManager.simulatedLayoutEnabled
                        onToggled: DisplayManager.simulatedLayoutEnabled = checked
                    }
                }
                ComboBox {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.leftMargin: 20
                    anchors.rightMargin: 20
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 6
                    height: 34
                    model: DisplayManager.displayNames
                    currentIndex: DisplayManager.selectedDisplayIndex
                    enabled: DisplayManager.displayNames.length > 1
                    onActivated: DisplayManager.selectedDisplayIndex = currentIndex
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: page.height < 720 ? 100 : 118
                color: Theme.surface
                radius: 14
                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 20
                    RowLayout {
                        Layout.fillWidth: true
                        Text { text: "Audio volume"; color: Theme.foreground; font.pixelSize: 15; font.weight: Font.DemiBold; Layout.fillWidth: true }
                        Text { text: Settings.volume + "%"; color: Theme.accent; font.pixelSize: 13 }
                    }
                    Slider { Layout.fillWidth: true; from: 0; to: 100; value: Settings.volume; onMoved: Settings.volume = Math.round(value) }
                }
            }
            Item { Layout.fillHeight: true }
            Text {
                text: "Preferences are stored locally using QSettings. This prototype does not connect to a vehicle."
                color: Theme.muted
                font.pixelSize: 11
            }
        }
    }
}
