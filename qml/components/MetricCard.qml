import QtQuick
import QtQuick.Layouts
import Auto.HMI 1.0

Rectangle {
    id: root
    property string label: ""
    property string value: ""
    property string detail: ""
    property string glyph: ""
    property color highlight: Theme.accent
    radius: 14
    color: Theme.surface
    border.color: Theme.elevated
    border.width: 1
    implicitHeight: 108

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 7
        RowLayout {
            Layout.fillWidth: true
            Text {
                text: root.label.toUpperCase()
                color: Theme.muted
                font.pixelSize: 10
                font.weight: Font.DemiBold
                font.letterSpacing: 0.8
                Layout.fillWidth: true
            }
            Text {
                text: root.glyph
                color: root.highlight
                font.pixelSize: 16
            }
        }
        Text {
            text: root.value
            color: Theme.foreground
            font.pixelSize: 22
            font.weight: Font.DemiBold
        }
        Text {
            text: root.detail
            color: Theme.muted
            font.pixelSize: 11
            visible: root.detail.length > 0
        }
    }
}
