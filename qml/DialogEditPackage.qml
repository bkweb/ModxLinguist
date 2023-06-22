import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property alias packageName: textFieldPackageName.text
    property alias author: textFieldAuthor.text
    property alias description: textFieldDescription.text
    title: qsTr("Edit Package")
    standardButtons: Dialog.Save | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        contentHeight: columnLayout.implicitHeight + 50
        GridLayout {
            id: columnLayout
            anchors.fill: parent
            rowSpacing: 5
            columns: 2
            // package name
            Label {
                Layout.alignment: Qt.AlignRight
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                text: qsTr("Package name")
            }
            CustomTextField {
                id: textFieldPackageName
                Layout.fillWidth: true
                Layout.rightMargin: 10
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                placeholderText: "myPackage"
            }
            // author
            Label {
                Layout.alignment: Qt.AlignRight
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                text: qsTr("Author")
            }
            CustomTextField {
                id: textFieldAuthor
                Layout.fillWidth: true
                Layout.rightMargin: 10
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                placeholderText: "Jean-Luc Picard"
            }
            // description
            Label {
                Layout.alignment: Qt.AlignRight
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                text: qsTr("Description")
            }
            CustomTextField {
                id: textFieldDescription
                Layout.fillWidth: true
                Layout.rightMargin: 10
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                placeholderText: ""
            }
        }
    }
}
