import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import org.lemuri.litteras 1.0

Window {
    width: 500
    height: 400
    visible: true
    title: qsTr("Accounts")

    color: sysPalette.window

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        TableView {
            width: 200
            TableViewColumn{ role: "name"  ; title: qsTr("Accounts") }
            model: AccountsEngine.accountsModel
            onCurrentRowChanged: {
                console.debug(currentRow)
            }
        }

        Rectangle {
            id: centerItem
            Layout.fillWidth: true
            color: "darkgray"

            GridLayout {
                anchors.left: parent.left
                anchors.right: parent.right
                columns: 2

                Label {
                    text: qsTr("Email address")
                }
                TextField {
                    id: email
                }

                Label {
                    text: qsTr("Full name")
                }
                TextField {
                    id: fullName
                }

                Label {
                    text: qsTr("Username")
                }
                TextField {
                    id: username
                }

                Label {
                    text: qsTr("Password")
                }
                TextField {
                    id: password
                    echoMode: TextInput.Password
                }

                Label {
                    text: qsTr("Internal server")
                }
                TextField {
                    id: internalServer
                }

                Label {
                    text: qsTr("External server")
                }
                TextField {
                    id: extServer
                }
            }
        }
    }
}
