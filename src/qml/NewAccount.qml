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

            warningLabel.text = qsTr("Authentication with \"%1\" has failed. Please check your username and password and try again.").arg(server)
        }
    }

    Text {
        anchors.bottom: parent.bottom
        opacity: 0.1
        font.pixelSize: parent.width / 2
        text: "@"
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10

        StackView {
            id: stackView
            Layout.alignment: Qt.AlignRight
            Layout.maximumWidth: parent.width * 0.7
            Layout.fillWidth: true
            Layout.fillHeight: true

            initialItem: GroupBox {
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !accountNew.processing
                title: qsTr("Setup new account")

                GridLayout {
                    anchors.fill: parent
                    columns: 3

                    Label {
                        Layout.columnSpan: 3
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        text: qsTr("Please fill the following fields to setup your mail account:")
                    }

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
                        source: "image://icon/arrow-left"
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
            }
        }

        RowLayout {
            Layout.columnSpan: 3

            Button {
                text: qsTr("Cancel")
                onClicked: root.destroy()
            }

            Item { Layout.fillWidth: true }

            Button {
                text: qsTr("Back")
                enabled: stackView.depth > 1
                onClicked: stackView.pop()
            }

            Button {
                id: nextButton
                isDefault: true
                text: accountNew.valid ? qsTr("Finish") : qsTr("Continue")
                onClicked: next()
            }
        }

        Keys.onEscapePressed: root.destroy()
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
