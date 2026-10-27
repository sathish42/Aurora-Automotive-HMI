import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window
import Auto.HMI 1.0

ApplicationWindow {
    id: window
    width: 1440
    height: 900
    minimumWidth: 1024
    minimumHeight: 640
    visible: true
    title: "AutoHMI"
    color: Theme.background

    function screenLabel(screen) {
        const labels = {
            Dashboard: "Overview",
            Cluster: "Instrument cluster",
            VehicleInfo: "Vehicle",
            Navigation: "Navigation",
            Climate: "Climate",
            Media: "Media",
            ADAS: "Driver assistance",
            Notifications: "Notifications",
            Diagnostics: "Diagnostics",
            Settings: "Settings"
        }
        return labels[screen] || screen
    }

    function screenGlyph(screen) {
        const glyphs = {
            Dashboard: "◫",
            Cluster: "◉",
            VehicleInfo: "◇",
            Navigation: "⌖",
            Climate: "☼",
            Media: "♫",
            ADAS: "◎",
            Notifications: "◌",
            Diagnostics: "⌕",
            Settings: "⚙"
        }
        return glyphs[screen] || "○"
    }

    function pageComponent(screen) {
        switch (screen) {
        case "Splash": return splashComponent
        case "Dashboard": return dashboardComponent
        case "Cluster": return clusterComponent
        case "VehicleInfo": return vehicleInfoComponent
        case "Navigation": return navigationComponent
        case "Climate": return climateComponent
        case "Media": return mediaComponent
        case "Settings": return settingsComponent
        case "Notifications": return notificationsComponent
        case "Diagnostics": return diagnosticsComponent
        case "ADAS": return adasComponent
        default: return dashboardComponent
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: Navigation.currentScreen === "Splash" ? 0 : 64
            visible: Navigation.currentScreen !== "Splash"
            color: Theme.surface

            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 24
                anchors.rightMargin: 28
                spacing: 16

                Text {
                    text: "A U R O R A"
                    color: Theme.foreground
                    font.pixelSize: 17
                    font.weight: Font.Bold
                    font.letterSpacing: 2
                }
                Rectangle {
                    width: 1
                    Layout.fillHeight: true
                    Layout.topMargin: 16
                    Layout.bottomMargin: 16
                    color: Theme.muted
                    opacity: 0.3
                }
                Text {
                    text: Navigation.currentScreen === "VehicleInfo" ? "Vehicle Information"
                          : Navigation.currentScreen
                    color: Theme.muted
                    font.pixelSize: 14
                    Layout.fillWidth: true
                }
                Text {
                    text: Vehicle.leftTurnSignal ? "◀" : " "
                    color: Theme.accent
                    font.pixelSize: 20
                }
                Text {
                    text: Qt.formatDateTime(new Date(), "ddd  hh:mm")
                    color: Theme.foreground
                    font.pixelSize: 14
                }
                Text {
                    text: Vehicle.rightTurnSignal ? "▶" : " "
                    color: Theme.accent
                    font.pixelSize: 20
                }
                Rectangle {
                    radius: 10
                    color: Theme.elevated
                    implicitWidth: statusLabel.implicitWidth + 24
                    implicitHeight: 32
                    Text {
                        id: statusLabel
                        anchors.centerIn: parent
                        text: "●  SIMULATION"
                        color: Theme.warning
                        font.pixelSize: 10
                        font.weight: Font.Bold
                    }
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Rectangle {
                Layout.fillHeight: true
                Layout.preferredWidth: window.width < 1200 ? 72 : 192
                visible: Navigation.currentScreen !== "Splash"
                color: Theme.surface

                ColumnLayout {
                    anchors.fill: parent
                    anchors.topMargin: 18
                    anchors.bottomMargin: 18
                    anchors.leftMargin: 12
                    anchors.rightMargin: 12
                    spacing: 5

                    Repeater {
                        model: Navigation.screens
                        delegate: Button {
                            id: navButton
                            required property string modelData
                            property bool selected: Navigation.currentScreen === modelData
                            Layout.fillWidth: true
                            Layout.preferredHeight: 46
                            flat: true
                            highlighted: selected
                            Accessible.name: window.screenLabel(navButton.modelData)
                            ToolTip.visible: window.width < 1200 && hovered
                            ToolTip.text: window.screenLabel(navButton.modelData)
                            onClicked: Navigation.navigate(modelData)
                            contentItem: RowLayout {
                                spacing: 12
                                Text {
                                    text: window.screenGlyph(navButton.modelData)
                                    color: navButton.selected ? Theme.accent : Theme.muted
                                    font.pixelSize: 19
                                    Layout.preferredWidth: 24
                                    horizontalAlignment: Text.AlignHCenter
                                }
                                Text {
                                    text: window.screenLabel(navButton.modelData)
                                    visible: window.width >= 1200
                                    color: navButton.selected ? Theme.foreground : Theme.muted
                                    font.pixelSize: 12
                                    font.weight: navButton.selected ? Font.DemiBold : Font.Normal
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }
                            }
                            background: Rectangle {
                                radius: 9
                                color: navButton.selected ? Theme.elevated : "transparent"
                                Rectangle {
                                    width: 3
                                    height: parent.height * 0.56
                                    anchors.left: parent.left
                                    anchors.verticalCenter: parent.verticalCenter
                                    radius: 2
                                    color: Theme.accent
                                    visible: navButton.selected
                                }
                            }
                        }
                    }

                    Item { Layout.fillHeight: true }
                    Text {
                        text: "DEMO BUILD  •  v1.0"
                        color: Theme.muted
                        opacity: 0.65
                        font.pixelSize: 9
                        Layout.leftMargin: 10
                    }
                }
            }

            StackView {
                id: screenStack
                objectName: "screenStack"
                Layout.fillWidth: true
                Layout.fillHeight: true
                initialItem: window.pageComponent(Navigation.currentScreen)
                replaceEnter: Transition {
                    NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 180 }
                }
                replaceExit: Transition {
                    NumberAnimation { property: "opacity"; from: 1; to: 0; duration: 120 }
                }
            }
        }
    }

    Component { id: splashComponent; SplashPage {} }
    Component { id: dashboardComponent; DashboardPage {} }
    Component { id: clusterComponent; ClusterPage {} }
    Component { id: vehicleInfoComponent; VehicleInfoPage {} }
    Component { id: navigationComponent; NavigationPage {} }
    Component { id: climateComponent; ClimatePage {} }
    Component { id: mediaComponent; MediaPage {} }
    Component { id: settingsComponent; SettingsPage {} }
    Component { id: notificationsComponent; NotificationsPage {} }
    Component { id: diagnosticsComponent; DiagnosticsPage {} }
    Component { id: adasComponent; ADASPage {} }

    Connections {
        target: Navigation
        function onCurrentScreenChanged() {
            const page = window.pageComponent(Navigation.currentScreen)
            if (page)
                screenStack.replace(page)
        }
    }

    InstrumentClusterWindow {}
}
