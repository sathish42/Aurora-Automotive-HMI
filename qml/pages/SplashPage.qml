import QtQuick
import QtQuick.Controls
import Auto.HMI 1.0

Rectangle {
    color: Theme.background
    Timer {
        interval: 1400
        running: true
        repeat: false
        onTriggered: Navigation.navigate("Dashboard")
    }
    Column {
        anchors.centerIn: parent
        spacing: 16
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "A U R O R A"
            color: Theme.foreground
            font.pixelSize: 34
            font.weight: Font.Bold
            font.letterSpacing: 5
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "CONNECTED MOBILITY"
            color: Theme.accent
            font.pixelSize: 11
            font.letterSpacing: 3
        }
        BusyIndicator {
            anchors.horizontalCenter: parent.horizontalCenter
            running: true
            palette.highlight: Theme.accent
        }
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: "Vehicle interface demonstration"
            color: Theme.muted
            font.pixelSize: 12
        }
    }
}
