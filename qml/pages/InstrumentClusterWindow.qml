import QtQuick
import QtQuick.Window
import Auto.HMI 1.0

Window {
    id: clusterWindow
    objectName: "secondaryClusterWindow"
    width: 900
    height: 560
    minimumWidth: 720
    minimumHeight: 440
    visible: Settings.clusterWindowEnabled && Qt.platform.os !== "android"
    title: "Aurora Automotive HMI — Secondary instrument display"
    color: Theme.background

    Component.onCompleted: DisplayManager.attachClusterWindow(clusterWindow)

    ClusterPage {
        anchors.fill: parent
    }
}
