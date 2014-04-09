import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {

    property alias text: textLabel.text
    ColumnLayout {
        anchors.centerIn: parent

        BusyIndicator {
            Layout.alignment: Qt.AlignHCenter
            id: busy
            running: true
        }

        Label {
            Layout.alignment: Qt.AlignHCenter
            Layout.fillWidth: true
            id: textLabel
            wrapMode: Text.WordWrap
        }
    }
}
