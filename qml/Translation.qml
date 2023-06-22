import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    property int entryId: 0
    property int topicId: 0
    property int languageId: 0
    property alias languageName: labelLanguageName.text
    property string translationKey: ""
    property alias translation: textAreaTranslation.text
    width: parent.width
    spacing: 10
    Label {
        id: labelLanguageName
        Layout.fillWidth: true
        font.bold: true
    }
    CustomTextArea {
        id: textAreaTranslation
        Layout.fillWidth: true
        placeholderText: qsTr("translation")
    }
}
