import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property var entryIds: []
    title: qsTr("Move Selected Translations")
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
    function setup(translationsArray) {
        for (var i = columnLayout.children.length - 1; i >= 0; i--) {
            columnLayout.children[i].destroy();
        }

        entryIds = [];
        var translationKeys = [];

        var customLabelComponent = Qt.createComponent("CustomLabel.qml");

        translationsArray.forEach((translationsItem) => {
            for (const languageName in translationsItem.translations) {
                var translation = translationsItem.translations[languageName];

                if (translation.entry_id > 0) {
                    entryIds.push(translation.entry_id);
                }

                if (translationKeys.indexOf(translation.translation_key) < 0) {
                    translationKeys.push(translation.translation_key);
                }
            }
        });

        translationKeys.forEach((translationKey) => {
            customLabelComponent.createObject(
                columnLayout, {
                    text: translationKey
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
            }
            Label {
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignCenter
                text: qsTr("Do you want to move the selected translations to the selected topic?")
            }
            ColumnLayout {
                id: columnLayout
            }
            Item {
                Layout.fillHeight: true
            }
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
