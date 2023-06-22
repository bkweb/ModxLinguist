import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property var entryIds: []
    title: qsTr("Move Translations")
    standardButtons: Dialog.Yes | Dialog.No
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    onOpened: {
        lexiconProcessor.getTopics();
    }
    Connections {
        target: lexiconProcessor
        ignoreUnknownSignals: true
        function onGetTopicsResult(topics) {
            topics = topics !== "" ? JSON.parse(topics) : [];

            listModelTopics.clear();
            topics.forEach(function(topic, index) {
                listModelTopics.append(topic);
            });
            comboBoxTopic.currentIndex = topics.length > 0 ? 0 : -1;
        }
    }
    function setup(translations) {
        entryIds = [];

        for (const languageName in translations.translations) {
            var translation = translations.translations[languageName];

            if (translation.entry_id > 0) {
                entryIds.push(translation.entry_id);
            }

            labelTranslationKey.text = translation.translation_key;
        }
    }
    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        spacing: 10
        RowLayout {
            Label {
                text: qsTr("Topic")
            }
            CustomComboBox {
                id: comboBoxTopic
                model: ListModel {
                    id: listModelTopics
                }
                textRole: "name"
                delegate: ItemDelegate {
                    text: model.name
                }
            }
            Label {
                Layout.leftMargin: 20
                text: qsTr("Translatation Key")
            }
            Label {
                id: labelTranslationKey
                font.bold: true
            }
        }
        Label {
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignCenter
            text: qsTr("Do you want to move the translations to the selected topic?")
        }
        Item {
            Layout.fillHeight: true
        }
    }
    function getEntryIds() {
        return entryIds;
    }
    function getTopic() {
        var topic = listModelTopics.get(comboBoxTopic.currentIndex);

        return topic;
    }
}
