import QtQuick 2.1
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
    width: ListView.view.width
    height: visible ? row.height + 4 : 0
    visible: roleIsVisible

    RowLayout {
        id: row
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.leftMargin: roleDepth * folderIcon.width

        Image {
            id: icon
            visible: roleChildrenCount
            height: width
            width: label.height
            sourceSize.height: height
            sourceSize.width: height
            source: roleExpanded ? "image://icon/arrow-down" : "image://icon/arrow-right"
            MouseArea {
                anchors.fill: parent
                onClicked: folderModel.toggleChildrenExpand(index)
            }
        }
        Item {
            width: label.height
            visible: !icon.visible
        }

        Image {
            id: folderIcon
            height: label.height
            width: height
            sourceSize.height: height
            sourceSize.width: height
            source: "image://icon/folder-grey"
            MouseArea {
                anchors.fill: parent
                onClicked:  {
                    console.debug("open " + index)
                }
                onDoubleClicked: folderModel.toggleChildrenExpand(index)
            }
        }
        Text {
            Layout.fillWidth: true
            id: label
            height: contentHeight
            elide: Text.ElideRight
            text: roleDisplayName
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.debug("open " + index)
                }
                onDoubleClicked: folderModel.toggleChildrenExpand(index)
            }
        }
    }


}
