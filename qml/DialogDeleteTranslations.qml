import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property var translations: []
    title: qsTr("Delete Translations")
    standardButtons: Dialog.Yes | Dialog.No
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    function setup(translations) {
        labelTranslationKey.text = translations.translation_key;
        dialog.translations = translations.translations;
    }
    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        spacing: 10
        Label {
            id: labelTranslationKey
        }
        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignCenter
            text: qsTr("Do you want to delete all related translations?")
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
