import QtQuick 2.15
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.15

import org.wpewebkit.qtwpe 1.0

ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("WPE WebKit Qt")

    ColumnLayout {
        anchors.fill: parent

        TextField {
            Layout.fillWidth: true
            Layout.preferredHeight: 60
            text: webView.url
        }

        WPEView {
            id: webView
            Layout.fillWidth: true
            Layout.fillHeight: true
            url: "https://google.com/"
        }
    }
}
