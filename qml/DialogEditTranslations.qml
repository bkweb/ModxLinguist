import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: qsTr("Edit Translations")
    standardButtons: Dialog.Save | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    function setup(translations) {
        for (var i = columnLayout.children.length - 1; i >= 1; i--) {
            columnLayout.children[i].destroy();
        }

        var translationComponent = Qt.createComponent("Translation.qml");

        for (const languageName in translations.translations) {
            var translation = translations.translations[languageName];

            translationComponent.createObject(
                        columnLayout, {
                            entryId: translation.entry_id,
                            languageId: translation.language_id,
                            languageName: translation.language_name,
                            topicId: translation.topic_id,
                            translationKey: translation.translation_key,
                            translation: translation.translation
                        });

            labelTopicName.text = translation.topic_name;
            textFieldTranslationKey.text = translation.translation_key;
        }
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
            id: columnLayout
            anchors.fill: parent
            anchors.rightMargin: 50
            spacing: 10
            RowLayout {
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                spacing: 30
                RowLayout {
                    Label {
                        text: qsTr("Topic")
                    }
                    Label {
                        id: labelTopicName
                        font.bold: true
                    }

                }
                ColumnLayout {
                    RowLayout {
                        Label {
                            text: qsTr("Translation Key")
                        }
                        CustomTextField {
                            id: textFieldTranslationKey
                            Layout.fillWidth: true
                            placeholderText: "default_my_key"
                            color: labelError.visible ? "orange" : ""
                            onTextChanged: {
                                lexiconProcessor.isAlreadyUsedTranslationKey(text);
                            }
                            Connections {
                                target: lexiconProcessor
                                ignoreUnknownSignals: true
                                function onIsAlreadyUsedTranslationKeyResult(topics) {
                                    topics = JSON.parse(topics);

                                    if (topics.length === 1) {
                                        if (topics[0].name === labelTopicName.text) {
                                            return;
                                        }
                                    }

                                    var topicNamesStr = "";
                                    for (var i = 0; i < topics.length; i++) {
                                        var topic = topics[i];

                                        topicNamesStr += topic.name;

                                        if (i + 1 < topics.length) {
                                            topicNamesStr += ", ";
                                        }
                                    }

                                    labelError.visible = topics.length > 0;
                                    labelError.text = qsTr("translation key is already used in: %1").arg(topicNamesStr);
                                }
                            }
                        }
                    }
                    Label {
                        id: labelError
                        Layout.fillWidth: true
                        color: "orange"
                        text: qsTr("translation key is already used in: %1")
                        wrapMode: Text.WordWrap
                        visible: false
                    }
                }
            }
        }
    }
    function getTranslationKey() {
        return textFieldTranslationKey.text;
    }
    function getTranslations() {
        var translations = [];

        for (var i = 1; i < columnLayout.children.length; i++) {
            var translation = columnLayout.children[i];

            translations.push({
                "entry_id": translation.entryId,
                "topic_id": translation.topicId,
                "language_id": translation.languageId,
                "translation_key": translation.translationKey,
                "translation": translation.translation
            });
        }

        return translations;
    }
}
