import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: dialog
    property alias topicName: textField.text
    title: qsTr("Add Topic")
    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true
    closePolicy: Popup.CloseOnEscape
    width: parent.width
    height: parent.height
    clip: true
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 10
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Topic")
            }
            CustomTextField {
                id: textField
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
                        var json = JSON.parse(data);

                        labelError.visible = json.isAlreadyUsedTopicName === true;
                    }
                }
            }
        }
        Label {
            id: labelError
            Layout.fillWidth: true
            color: "red"
            text: qsTr("topic name is already used")
            wrapMode: Text.WordWrap
            visible: false
        }
        Item {
            Layout.fillHeight: true
        }
    }
}
