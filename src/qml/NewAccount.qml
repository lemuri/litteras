import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.lemuri.litteras 1.0

Window {
    id: root
    width: 500
    height: 400
    visible: true
    title: qsTr("New account")

    color: sysPalette.window

    property bool hideUsername: true

    AccountNew {
        id: accountNew
        onAuthenticationError: {
            hideUsername = false
            username.text = accountNew.username
            password.selectAll()
            password.forceActiveFocus()
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: accountNew.processing
    }

    GroupBox {
        anchors.fill: parent
        visible: !accountNew.processing
        title: qsTr("Welcome to Litteras")

        GridLayout {
            anchors.fill: parent
            columns: 3

            Label {
                Layout.fillWidth: true
                Layout.columnSpan: 3
                text: qsTr("Please fill the following fields to setup your mail account:")
                wrapMode: Text.WordWrap
            }

            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("Full Name")
            }
            TextField {
                Layout.fillWidth: true
                id: fullName
                onTextChanged: accountNew.fullName = text
            }

            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("Email Address")
            }
            TextField {
                Layout.fillWidth: true
                id: email
                onTextChanged: accountNew.emailAddress = text
            }

            Item { Layout.fillWidth: true; visible: !hideUsername }
            Label {
                visible: !hideUsername
                text: qsTr("Username")
            }
            TextField {
                Layout.fillWidth: true
                id: username
                visible: !hideUsername
                onTextChanged: accountNew.username = text
            }

            Item { Layout.fillWidth: true }
            Label {
                text: qsTr("Password")
            }
            TextField {
                Layout.fillWidth: true
                id: password
                echoMode: TextInput.Password
                onTextChanged: accountNew.password = text
            }

            Item {
                Layout.fillHeight: true
                Layout.columnSpan: 3
            }


            RowLayout {
                Layout.columnSpan: 3

                Button {
                    iconName: "dialog-cancel"
                    text: qsTr("Cancel")
                    onClicked: root.destroy()
                }

                Item { Layout.fillWidth: true }

                Button {
                    iconName: "dialog-ok"
                    text: accountNew.valid ? qsTr("Finish") : qsTr("Continue")
                    onClicked: {
                        if (accountNew.valid) {
                            accountNew.save()
                            root.destroy()
                        } else {
                            accountNew.process()
                        }
                    }
                }
            }
        }
    }
}
