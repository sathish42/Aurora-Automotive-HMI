import QtQuick
import QtQuick.Layouts
import Auto.HMI 1.0

ColumnLayout {
    property string title: ""
    property string subtitle: ""
    spacing: 5
    Text {
        text: parent.title
        color: Theme.foreground
        font.pixelSize: 25
        font.weight: Font.DemiBold
    }
    Text {
        text: parent.subtitle
        color: Theme.muted
        font.pixelSize: 12
    }
}
