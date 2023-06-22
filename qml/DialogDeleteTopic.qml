import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: qsTr("Delete Topic")
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
    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        spacing: 10
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
            Layout.fillWidth: true
            horizontalAlignment: Text.AlignCenter
            text: qsTr("Do you want to delete the selected topic and all related translations?")
        }
        Item {
            Layout.fillHeight: true
        }
    }
    function getTopicId() {
        var topic = listModelTopics.get(comboBoxTopic.currentIndex);

        if (typeof topic !== 'undefined') {
            return topic.id;
        }

        return -1;
    }
}
