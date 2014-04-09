import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import org.lemuri.litteras 1.0

Window {
    id: root
    width: 500
    height: 400
    modality: Qt.WindowModal
    title: AccountsEngine.count ? qsTr("Add Account") : qsTr("Welcome to Litteras")

    color: sysPalette.window

    property bool hideUsername: true
    onHideUsernameChanged: {
        if (hideUsername) {
            stackView.currentItem
        }
    }
    onVisibleChanged: {
        if (!visible) {
            destroy();
        }
    }

    function next() {
        if (accountNew.valid) {
            accountNew.save()
            root.destroy()
        } else {
            accountNew.process()
        }
    }

    AccountNew {
        id: accountNew
        onAuthenticationError: {
            hideUsername = false
            username.text = accountNew.username
            username.selectAll()
            username.forceActiveFocus()

            warningLabel.text = qsTr("Authentication with \"%1\" has failed. Please check you username and password and try again.").arg(server)
            warningLabel.visible = true
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        StackView {
            id: stackView
            Layout.fillWidth: true
            Layout.fillHeight: true

            initialItem: GroupBox {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !accountNew.processing
                title: qsTr("Please fill the following fields to setup your mail account:")

                GridLayout {
                    anchors.fill: parent
                    columns: 3

                    Label {
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Full Name")
                    }
                    TextField {
                        Layout.fillWidth: true
                        id: fullName
                        text: accountNew.fullName
                        onTextChanged: accountNew.fullName = text
                    }
                    Item { width: fullName.height }

                    Label {
                        Layout.alignment: Qt.AlignRight
                        text: qsTr("Email Address")
                    }
                    TextField {
                        Layout.fillWidth: true
                        id: email
                        onTextChanged: accountNew.emailAddress = text
                    }
                    Item { width: fullName.height }

                    Label {
                        Layout.alignment: Qt.AlignRight
                        visible: !hideUsername
                        text: qsTr("Username")
                    }
                    TextField {
                        Layout.fillWidth: true
                        id: username
                        visible: !hideUsername
                        onTextChanged: accountNew.username = text
                    }
                    Image {
                        Layout.maximumWidth: height
                        id: icon
                        visible: !hideUsername
                        height: username.height
                        width: height
                        sourceSize.height: height
                        sourceSize.width: height
                        source: "image://icon/task-attempt"
                    }

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
                    Item { width: fullName.height }

                    Item {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        Layout.columnSpan: 3
                        Row {
                            anchors.bottom: parent.bottom
                            anchors.horizontalCenter: parent.horizontalCenter
                            visible: accountNew.processing

                            BusyIndicator {
                                id: busy
                                height: busyLabel.height
                                running: parent.visible
                            }
                            Label {
                                id: busyLabel
                                width: contentWidth
                                text: qsTr("Please wait while we try to get in contact with the mail servers")
                            }
                        }
                    }

                    Image {
                        Layout.maximumWidth: height
                        Layout.alignment: Qt.AlignRight | Qt.AlignTop
                        height: username.height
                        width: height
                        visible: warningLabel.visible
                        sourceSize.height: height
                        sourceSize.width: height
                        source: "image://icon/task-attention"
                    }
                    Label {
                        Layout.columnSpan: 2
                        Layout.fillWidth: true
                        id: warningLabel
                        visible: false
                        width: contentWidth
                        wrapMode: Text.WordWrap
                    }
                }
            }
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
                iconName: "go-previous-view"
                text: qsTr("Back")
                enabled: stackView.depth > 1
                onClicked: stackView.pop()
            }

            Button {
                id: nextButton
                iconName: "go-next-view"
                isDefault: true
                text: accountNew.valid ? qsTr("Finish") : qsTr("Continue")
                onClicked: next()
            }
        }
    }

    Component.onCompleted: {
        visible = true
        email.forceActiveFocus()
    }

    Component.onDestruction: {
        if (!AccountsEngine.hadAccounts) {
            Qt.quit()
        }
    }
}
