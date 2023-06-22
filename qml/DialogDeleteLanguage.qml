import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: qsTr("Delete Language")
    standardButtons: Dialog.Yes | Dialog.No
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width * 0.66
    height: parent.height * 0.5
    clip: true
    onOpened: {
        lexiconProcessor.getLanguages();
    }
    Connections {
        target: lexiconProcessor
        ignoreUnknownSignals: true
        function onGetLanguagesResult(languages) {
            languages = languages !== "" ? JSON.parse(languages) : [];

            listModelLanguages.clear();
            languages.forEach(function(language, index) {
                listModelLanguages.append(language);
            });
            comboBoxLanguage.currentIndex = languages.length > 0 ? 0 : -1;
        }
    }
    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10
        CustomComboBox {
            id: comboBoxLanguage
            model: ListModel {
                id: listModelLanguages
            }
            textRole: "name"
            delegate: ItemDelegate {
                text: model.name
            }
        }
        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignCenter
            wrapMode: Qt.TextWordWrap
            text: qsTr("Do you want to delete the selected language and all related translations?")
        }
        Item {
            Layout.fillHeight: true
        }
    }
    function getLanguageId() {
        var language = listModelLanguages.get(comboBoxLanguage.currentIndex);

        if (typeof language !== 'undefined') {
            return language.id;
        }

        return -1;
    }
}
