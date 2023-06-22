import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property var languages: []
    property var topic: null
    title: qsTr("Add Translations")
    standardButtons: Dialog.Save | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    onOpened: {
        lexiconProcessor.getLanguages();
    }
    Connections {
        target: lexiconProcessor
        ignoreUnknownSignals: true
        function onGetLanguagesResult(languages) {
            dialog.languages = languages !== "" ? JSON.parse(languages) : [];

            generateTranslations();
        }
    }
    function setup(topic) {
        dialog.topic = topic;

        for (var i = columnLayout.children.length - 1; i >= 1; i--) {
            columnLayout.children[i].destroy();
        }
    }
    function generateTranslations() {
        if ((languages.length == 0) && !topic) {
            return;
        }

        var translationComponent = Qt.createComponent("Translation.qml");

        languages.forEach(function(language, index) {
            translationComponent.createObject(
                        columnLayout, {
                            entryId: 0,
                            languageId: language.id,
                            languageName: language.name,
                            topicId: topic.id,
                            translationKey: "",
                            translation: ""
                        });
        });

        labelTopicName.text = topic.name;
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
    function getTranslation() {
        var translations = [];

        for (var i = 1; i < columnLayout.children.length; i++) {
            var translation = columnLayout.children[i];

            translations.push({
                "entry_id": translation.entryId,
                "topic_id": translation.topicId,
                "language_id": translation.languageId,
                "translation_key": textFieldTranslationKey.text,
                "translation": translation.translation
            });
        }

        return translations;
    }
}
