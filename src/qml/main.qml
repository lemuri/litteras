import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.lemuri.litteras 1.0

ApplicationWindow {
    id: mainWindow
    width: 800
    height: 600

    title: "Litteras"

    property var newAccount: null

    function addAccount() {
        if (!newAccount) {
            var component = Qt.createComponent("NewAccount.qml")
            if (component.status === Component.Ready) {
                newAccount = component.createObject(mainWindow)
            } else {
                console.debug(component.errorString())
            }
        }
    }

    SystemPalette { id: sysPalette }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&File")
            MenuItem {
                text: qsTr("&Exit")
                onTriggered: Qt.quit()
            }
        }
        Menu {
            title: qsTr("Settings")
            MenuItem {
                text: qsTr("New account")
                onTriggered: addAccount()
            }
            MenuItem {
                text: qsTr("Manage accounts")
                onTriggered: {
                    var component = Qt.createComponent("Accounts.qml")
                    if (component.status === Component.Ready) {
                        component.createObject(mainWindow)
                    } else {
                        console.debug(component.errorString())
                    }
                }
            }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            anchors.fill: parent

            ToolButton {
                iconName: "mail-send"
                onClicked: {

                }
            }

            ToolButton {
                iconName: "mail-receive"
            }

            Item {
                Layout.fillWidth: true
            }

            TextField {
                id: searchText
                focus: true
            }
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Rectangle {
            width: 200
            Layout.maximumWidth: 400
            color: "gray"

            ScrollView {
                anchors.fill: parent
                ListView {
                    id: folderView
                    model: FolderModel {
                        id: folderModel
                    }
                    delegate: FolderDelegate {
                    }

                    section.property: "roleHeaderSection"
                    section.delegate: Text {
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.margins: 4
                        height: contentHeight + 8
                        verticalAlignment: Text.AlignVCenter
                        opacity: 0.4
                        font.bold: true
                        elide: Text.ElideRight
                        text: section
                    }
                }
            }
        }
        Rectangle {
            id: centerItem
            Layout.minimumWidth: 50
            Layout.fillWidth: true
            color: "darkgray"
        }
        Rectangle {
            width: 200
            color: "gray"
        }
    }

    Component.onCompleted: {
        mainWindow.visible = true
        if (AccountsEngine.count === 0) {
            addAccount()
        }
    }
}
