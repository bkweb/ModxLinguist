import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property var translations: []
    title: qsTr("Delete Selected Translations")
    standardButtons: Dialog.Yes | Dialog.No
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    function setup(translations) {
        for (var i = columnLayout.children.length - 1; i >= 0; i--) {
            columnLayout.children[i].destroy();
        }

        dialog.translations = translations;

        var customLabelComponent = Qt.createComponent("CustomLabel.qml");

        translations.forEach((translation, index) => {
            customLabelComponent.createObject(
                        columnLayout, {
                            text: translation.translation_key
                        });
        });
    }
    Flickable {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.topMargin: 10
        anchors.bottomMargin: 10
        flickableDirection: Flickable.VerticalFlick
        contentHeight: columnLayout.implicitHeight + 50
        ScrollBar.vertical: ScrollBar {
            id: scrollBarEntries
            width: 25
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        ColumnLayout {
            anchors.fill: parent
            spacing: 10
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignCenter
                text: qsTr("Do you want to delete the selected translations?")
            }
            ColumnLayout {
                id: columnLayout
            }
            Item {
                Layout.fillHeight: true
            }
        }
    }
}
