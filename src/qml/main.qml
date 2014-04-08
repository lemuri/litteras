import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import org.lemuri.litteras 1.0

ApplicationWindow {
    id: mainWindow
    width: 800
    height: 600
    visible: true

    title: "Litteras"

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
                onTriggered: {
                    var component = Qt.createComponent("NewAccount.qml")
                    if (component.status === Component.Ready) {
                        component.createObject(mainWindow)
                    } else {
                        console.debug(component.errorString())
                    }
                }
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
                    model: FolderModel {}
                    delegate: Text {
                        text: model.roleDisplayName
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
}
