import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    title: qsTr("Edit Topic")
    standardButtons: Dialog.Save | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    function setUp(topics) {
        listModelTopics.clear();
        topics.forEach(function(topic, index) {
            listModelTopics.append(topic);
        });
        comboBoxTopic.currentIndex = topics.length > 0 ? 0 : -1;
    }
    GridLayout {
        anchors.fill: parent
        anchors.margins: 10
        columns: 2
        rowSpacing: 10
        Label {
            Layout.columnSpan: 2
            text: qsTr("Select the topic whose name you want to change.")
        }
        // select topic
        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Topic")
        }
        CustomComboBox {
            id: comboBoxTopic
            Layout.fillWidth: true
            model: ListModel {
                id: listModelTopics
            }
            textRole: "name"
            delegate: ItemDelegate {
                text: model.name
            }
        }
        // topic name
        Label {
            Layout.alignment: Qt.AlignRight
            text: qsTr("New Topic Name")
        }
        CustomTextField {
            id: textFieldTopicName
            Layout.fillWidth: true
            placeholderText: "topic_name"
            color: labelError.visible ? "red" : ""
            onTextChanged: {
                lexiconProcessor.isAlreadyUsedTopicName(text);
            }
            Connections {
                target: lexiconProcessor
                ignoreUnknownSignals: true
                function onIsAlreadyUsedTopicNameResult(data) {
                    data = JSON.parse(data);

                    labelError.visible = data.isAlreadyUsedTopicName
                }
            }
        }
        Item {}
        Label {
            id: labelError
            color: "red"
            text: qsTr("topic name is already used")
            visible: false
        }
        Item {
            Layout.columnSpan: 2
            Layout.fillHeight: true
        }
    }
    function getTopic() {
        var topic = listModelTopics.get(comboBoxTopic.currentIndex);

        if (topic !== 'undefined') {
            return {
                "id": topic.id,
                "name": textFieldTopicName.text
            };
        }

        return {
            "id": 0,
            "name": ""
        };
    }
}
