import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.lemuri.litteras 1.0

GroupBox {
    Layout.fillWidth: true
    Layout.fillHeight: true
    visible: !accountNew.processing
    title: qsTr("Mail Server")

    GridLayout {
        Layout.fillWidth: true
        Layout.fillHeight: true
        columns: 3

        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Account Type")
        }
        ComboBox {
            Layout.fillWidth: true
            id: accountKind
            model: ["Exchange Server"]
        }
        Item { width: fullName.height }

        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Description")
        }
        TextField {
            Layout.fillWidth: true
            id: description
            onTextChanged: accountNew.emailAddress = text
        }
        Item { width: fullName.height }

        Item { Layout.columnSpan: 3; height: description.height }

        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Server Address")
        }
        TextField {
            Layout.fillWidth: true
            id: serverAddress
            onTextChanged: accountNew.emailAddress = text
        }
        Item { width: description.height }

        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Username")
        }
        TextField {
            Layout.fillWidth: true
            id: username
            onTextChanged: accountNew.username = text
        }
        Item { width: description.height }

        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Password")
        }
        TextField {
            Layout.fillWidth: true
            id: password
            echoMode: TextInput.Password
            onTextChanged: accountNew.password = text
            onAccepted: next()
        }
        Item { width: description.height }

        Item {
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.columnSpan: 3
        }

        RowLayout {
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            visible: accountNew.processing

            BusyIndicator {
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: fullName.height
                id: busy
                running: parent.visible
            }
            Label {
                Layout.fillWidth: true
                id: busyLabel
                wrapMode: Text.WordWrap
                text: qsTr("Please wait while we try to get in contact with the mail servers")
            }
        }

        RowLayout {
            id: warningRow
            Layout.columnSpan: 3
            Layout.alignment: Qt.AlignCenter
            visible: !accountNew.processing && warningLabel.text.length

            Image {
                Layout.alignment: Qt.AlignTop
                Layout.maximumWidth: fullName.height
                height: username.height
                width: height
                sourceSize.height: height
                sourceSize.width: height
                source: "image://icon/task-attention"
            }
            Label {
                Layout.fillWidth: true
                id: warningLabel
                wrapMode: Text.WordWrap
            }
        }
    }

    Component.onCompleted: {
        username.text = accountNew.username
        password.text = accountNew.password
        serverAddress.text = accountNew.serverAddress
    }
}
