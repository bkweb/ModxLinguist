import QtQuick
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import ModxLinguist.LexiconProcessor 1.0

ApplicationWindow {
    id: window
    property var languages: []
    width: 960
    height: 720
    visible: true
    title: qsTr("ModxLinguist")
    BusyIndicator {
        parent: Overlay.overlay
        anchors.centerIn: parent
        width: 0.1 * window.width
        height: 0.1 * window.width
        z: 100
        running: lexiconProcessor.loading
        visible: running
    }
    MessagePopup {
        id: messagePopup
        parent: Overlay.overlay
        anchors.centerIn: parent
    }
    FolderDialog {
        id: folderDialogImportFromLexiconDirectory
        onAccepted: {
            lexiconProcessor.importFromLexiconDirectory(currentFolder);
        }
    }
    FolderDialog {
        id: folderDialogExportAsFileStructure
        onAccepted: {
            lexiconProcessor.exportAsFileStructure(currentFolder);
        }
    }
    FolderDialog {
        id: folderDialogOpenLexiconDirectory
        onAccepted: {
            resetPackage();

            // remove prefix for windows
            var path = currentFolder.toString().replace("file:///", "");

            labelLexiconDirectory.text = path;
            lexiconProcessor.updateSetting(lexiconProcessor.getSettingsKey(lexiconProcessor.SelectedLexiconDirectoryPath), path);

            lexiconProcessor.importFromLexiconDirectory(currentFolder);
        }
    }
    DialogAddLanguage {
        id: dialogAddLanguage
        onAccepted: {
            lexiconProcessor.addLanguage(languageName);
        }
    }
    DialogDeleteLanguage {
        id: dialogDeleteLanguage
        onAccepted: {
            var languageId = getLanguageId();

            if (languageId > 0) {
                lexiconProcessor.deleteLanguage(languageId);
            }
        }
    }
    DialogAddTopic {
        id: dialogAddTopic
        onAccepted: {
            lexiconProcessor.addTopic(topicName);
        }
    }
    DialogEditTopic {
        id: dialogEditTopic
        onAccepted: {
            var topic = getTopic();

            lexiconProcessor.updateTopic(topic.id, topic.name);
        }
    }
    DialogDeleteTopic {
        id: dialogDeleteTopic
        onAccepted: {
            var topicId = getTopicId();

            if (topicId > 0) {
                lexiconProcessor.deleteTopic(topicId);
            }
        }
    }
    DialogEditPackage {
        id: dialogEditPackage
        onAccepted: {
            lexiconProcessor.updatePackage(packageName, description, author, 0);
        }
    }
    DialogAddTranslations {
        id: dialogAddTranslations
        onAccepted: {
            var translations = getTranslation();

            translations.forEach(function(translation, index) {
                lexiconProcessor.addEntry(translation.language_id, translation.topic_id, translation.translation_key, translation.translation);
            });

            // refresh listModelEntries to get the updated translation values and scroll listViewEntries to edited entry
            var topic = listModelTopics.get(comboBoxTopic.currentIndex);
            lexiconProcessor.getTopicEntries(topic.id);
        }
    }
    DialogEditTranslations {
        id: dialogEditTranslations
        onAccepted: {
            var translationKey = getTranslationKey();
            var translations = getTranslations();

            translations.forEach(function(translation, index) {
                if (translation.entry_id > 0) {
                    lexiconProcessor.updateEntry(translation.entry_id, translationKey, translation.translation);
                } else {
                    lexiconProcessor.addEntry(translation.language_id, translation.topic_id, translationKey, translation.translation);
                }
            });

            // refresh listModelEntries to get the updated translation values and scroll listViewEntries to edited entry
            var topic = listModelTopics.get(comboBoxTopic.currentIndex);
            lexiconProcessor.getTopicEntries(topic.id);
        }
    }
    DialogMoveTranslations {
        id: dialogMoveTranslations
        onAccepted: {
            var entryIds = getEntryIds();
            var targetTopic = getTopic();

            entryIds.forEach((entryId) => {
                lexiconProcessor.moveEntry(entryId, targetTopic.id);
            });

            // refresh listModelEntries and scroll listViewEntries
            var topic = listModelTopics.get(comboBoxTopic.currentIndex);
            lexiconProcessor.getTopicEntries(topic.id);
        }
    }
    DialogMoveSelectedTranslations {
        id: dialogMoveSelectedTranslations
        onAccepted: {
            var entryIds = getEntryIds();
            var targetTopic = getTopic();

            entryIds.forEach((entryId) => {
                lexiconProcessor.moveEntry(entryId, targetTopic.id);
            });

            // refresh listModelEntries and scroll listViewEntries
            var listModelIndex = -1;
            for (var i = 0; i < listModelTopics.count; i++) {
                var topic = listModelTopics.get(i);
                if (topic.id === targetTopic.id) {
                    listModelIndex = i;
                    break;
                }
            }

            if (listModelIndex >= 0) {
                comboBoxTopic.currentIndex = listModelIndex;
                lexiconProcessor.getTopicEntries(targetTopic.id);
            }
        }
    }
    MessageDialog {
        id: messageDialogNewPackage
        title: qsTr("New Package")
        text: qsTr("If you create a new package, all data of the current package will be deleted. Do you want to proceed?")
        buttons: Dialog.Yes | Dialog.No
        modality: Qt.ApplicationModal
        onAccepted: {
            resetPackage();
        }
    }
    MessageDialog {
        id: messageDialogExportAsFileStructure
        title: qsTr("Export as Lexicon File Structure")
        text: qsTr("Translations are missing. Do you still want to proceed?")
        buttons: Dialog.Yes | Dialog.No
        modality: Qt.ApplicationModal
        onAccepted: {
            folderDialogExportAsFileStructure.open();
        }
    }
    DialogDeleteTranslations {
        id: dialogDeleteTranslations
        onAccepted: {
            listViewEntries.checkedListItems = [];
            listViewEntries.checkedItemsChanged();

            for (const languageName in translations) {
                var translation = translations[languageName];

                if (translation.entry_id > 0) {
                    lexiconProcessor.deleteEntry(translation.entry_id);
                }
            };
        }
    }
    DialogDeleteSelectedTranslations {
        id: dialogDeleteSelectedTranslations
        onAccepted: {
            listViewEntries.checkedListItems = [];
            listViewEntries.checkedItemsChanged();

            var translationsArray = dialogDeleteSelectedTranslations.translations;

            translationsArray.forEach((translations, index) => {
                for (const languageName in translations.translations) {
                    var translation = translations.translations[languageName];

                    if (translation.entry_id > 0) {
                        lexiconProcessor.deleteEntry(translation.entry_id);
                    }
                };
            });
        }
    }
    DialogAbout {
        id: dialogAbout
    }
    Component.onCompleted: {
        // try to load from database file
        lexiconProcessor.getTopics();

        lexiconProcessor.getSetting(lexiconProcessor.getSettingsKey(lexiconProcessor.SelectedLexiconDirectoryPath));
    }
    function resetPackage() {
        // reset package
        listModelEntries.clear();
        listViewEntries.checkedListItems = [];
        listViewEntries.checkedItemsChanged();

        lexiconProcessor.emptyDatabase();

        lexiconProcessor.updateSetting(lexiconProcessor.getSettingsKey(lexiconProcessor.SelectedLexiconDirectoryPath), "");
    }

    Connections {
        target: lexiconProcessor
        ignoreUnknownSignals: true
        function onImportFromLexiconDirectoryFinished() {
            console.log("import processing finished");
            lexiconProcessor.getTopics();
        }
        function onExportAsFileStructureFinished() {
            console.log("export processing finished");

            messagePopup.show(qsTr("Exported"));
        }
        function onSaveTopicToFileStructureFinished() {
            console.log("saving topic to file finished");

            messagePopup.show(qsTr("Saved"));
        }
        function onUpdatedDatabaseStats(stats) {
            stats = JSON.parse(stats);

            labelLanguageCount.text = qsTr("languages: %1").arg(stats.languageCount);
            labelTopicCount.text = qsTr("topics: %1").arg(stats.topicCount);
            labelEntryCount.text = qsTr("entries: %1").arg(stats.entryCount);
            if (stats.missingTranslationCount > 0) {
                buttonMissingTranslationCount.text = qsTr("missing translations: %1").arg(stats.missingTranslationCount);
            } else {
                buttonMissingTranslationCount.text = "";
            }

            window.languages = stats.languages;

            dialogEditPackage.packageName = stats.packageName;
            dialogEditPackage.author = stats.author;
            dialogEditPackage.description = stats.description;
        }
        function onUpdatedProcessingFile(fileName) {
            labelProcessing.text = qsTr("processing: %1").arg(fileName)
        }
        function onGetTopicsResult(topics) {
            topics = topics !== "" ? JSON.parse(topics) : [];

            listModelTopics.clear();
            topics.forEach(function(topic, index) {
                listModelTopics.append(topic);
            });
            comboBoxTopic.currentIndex = topics.length > 0 ? 0 : -1;

            dialogEditTopic.setUp(topics);
        }
        function onGetTopicEntriesResult(entries) {
            var json = JSON.parse(entries);

            listViewEntries.processEntries(json);
        }
        function onSearchEntriesResult(entries) {
            var json = JSON.parse(entries);

            listViewEntries.processEntries(json);
        }
        function onGetMissingTranslationsResult(missingTranslations) {
            var json = JSON.parse(missingTranslations);

            listViewEntries.processEntries(json);
        }
        function onAddEntryResult(entryId) {
            console.log("added entry:", entryId);
        }
        function onUpdateEntryResult(entryId) {
            console.log("updated entry:", entryId);
        }
        function onMoveEntriesResult(numRowsAffected) {
            console.log("moved entry count:", numRowsAffected);
        }
        function onDeleteEntryResult(entryId) {
            console.log("deleted entry:", entryId);
            lexiconProcessor.getTopics();
        }
        function onDeleteLanguageResult(languageId) {
            console.log("deleted language:", languageId);
            comboBoxTopic.update();
        }
        function onAddLanguageResult(languageId) {
            console.log("language added:", languageId);
            comboBoxTopic.update();
        }
        function onAddTopicResult(topicId) {
            console.log("topic added:", topicId);
        }
        function onDeleteTopicResult(topicId) {
            console.log("topic deleted:", topicId);
        }
        function onUpdatePackageResult(data) {
            var json = JSON.parse(data);

            dialogEditPackage.packageName = json.name;
            dialogEditPackage.author = json.author;
            console.log("package updated", data);
        }
        function onUpdateSettingResult(data) {
            var json = JSON.parse(data);

            console.log("setting updated", data);

            if (json.key === lexiconProcessor.getSettingsKey(lexiconProcessor.SelectedLexiconDirectoryPath)) {
                labelLexiconDirectory.text = json.value;
            }
        }
        function onMoveEntry(entryId) {
            console.log("moved entry:", entryId);
        }
        function onGetSettingResult(setting) {
            var json = JSON.parse(setting);

            console.log("setting:", setting);

            if (json.key === lexiconProcessor.getSettingsKey(lexiconProcessor.SelectedLexiconDirectoryPath)) {
                labelLexiconDirectory.text = json.value;
            }
        }
    }

    menuBar: MenuBar {
        Menu {
            title: qsTr("&Package")
            Action {
                text: qsTr("&New Package")
                onTriggered: messageDialogNewPackage.open()
            }
            Action {
                text: qsTr("&Edit Package")
                onTriggered: dialogEditPackage.open()
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Open Lexicon Directory")
                onTriggered: folderDialogOpenLexiconDirectory.open()
            }
            Action {
                shortcut: "Ctrl+S"
                text: qsTr("&Save Current Topic")
                onTriggered: {
                    if (labelLexiconDirectory.text.length > 0) {
                        var topic = listModelTopics.get(comboBoxTopic.currentIndex);

                        lexiconProcessor.saveTopicToFileStructure(topic.id, labelLexiconDirectory.text);
                    }
                }
            }
            Action {
                text: qsTr("Save All Topics")
                onTriggered: {
                    if (labelLexiconDirectory.text.length > 0) {
                        lexiconProcessor.exportAsFileStructure(labelLexiconDirectory.text);
                    }
                }
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Import from Lexicon Directory")
                onTriggered: folderDialogImportFromLexiconDirectory.open()
            }
            Action {
                text: qsTr("&Export as Lexicon File Structure")
                onTriggered: {
                    if (buttonMissingTranslationCount.visible) {
                        messageDialogExportAsFileStructure.open();
                    } else {
                        folderDialogExportAsFileStructure.open();
                    }
                }
            }
            MenuSeparator { }
            Action {
                text: qsTr("&Quit")
                onTriggered: window.close()
            }
        }
        Menu {
            title: qsTr("&Lexicon")
            Action {
                text: qsTr("Add Translation")
                onTriggered: {
                    var topic = listModelTopics.get(comboBoxTopic.currentIndex);

                    dialogAddTranslations.setup(topic);
                    dialogAddTranslations.open();
                }
            }
            Action {
                text: qsTr("Find Missing Translations")
                onTriggered: {
                    lexiconProcessor.getMissingTranslations();
                }
            }
            MenuSeparator { }
            Action {
                text: qsTr("Add &Topic")
                onTriggered: dialogAddTopic.open()
            }
            Action {
                text: qsTr("Edit Topic")
                onTriggered: dialogEditTopic.open()
            }
            Action {
                text: qsTr("Delete Topic")
                onTriggered: dialogDeleteTopic.open()
            }
            MenuSeparator { }
            Action {
                text: qsTr("Add &Language")
                onTriggered: dialogAddLanguage.open()
            }
            Action {
                text: qsTr("Delete Language")
                onTriggered: dialogDeleteLanguage.open()
            }
        }
        Menu {
            title: qsTr("&Help")
            Action {
                text: qsTr("&About")
                onTriggered: dialogAbout.open()
            }
        }
    }

    footer: ToolBar {
        RowLayout {
            anchors.fill: parent
            RowLayout {
                spacing: 10
                Label {
                    id: labelLanguageCount
                }
                Label {
                    id: labelTopicCount
                }
                Label {
                    id: labelEntryCount
                }
                CustomButton {
                    id: buttonMissingTranslationCount
                    visible: text !== ""
                    onClicked: {
                        lexiconProcessor.getMissingTranslations();
                    }
                }
                BusyIndicator {
                    id: busyIndicator
                    running: lexiconProcessor.loading
                }
                Label {
                    id: labelProcessing
                    visible: lexiconProcessor.loading
                }
                Label {
                    id: labelCheckedItems
                    text: qsTr("press the right mouse button to display options")
                    visible: false
                }
            }
        }
    }

    header: ToolBar {
        ColumnLayout {
            anchors.fill: parent
            RowLayout {
                Layout.fillWidth: true
                spacing: 0
                RowLayout {
                    Layout.margins: 10
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
                        onCurrentIndexChanged: {
                            update();
                        }
                        function update() {
                            if (currentIndex >= 0) {
                                var topic = listModelTopics.get(currentIndex);
                                lexiconProcessor.getTopicEntries(topic.id);
                            }
                        }
                    }
                    CustomButton {
                        text: qsTr("Add Translation")
                        onClicked: {
                            var topic = listModelTopics.get(comboBoxTopic.currentIndex);

                            dialogAddTranslations.setup(topic);
                            dialogAddTranslations.open();
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                RowLayout {
                    CustomTextField {
                        id: textFieldSearchTerm
                        Layout.preferredWidth: window.width * 0.2
                        placeholderText: qsTr("Search")
                        onAccepted: {
                            lexiconProcessor.searchEntries(text);
                        }
                    }
                    CustomButton {
                        text: qsTr("Search")
                        onClicked: {
                            lexiconProcessor.searchEntries(textFieldSearchTerm.text);
                        }
                    }
                }
            }
            RowLayout {
                Layout.bottomMargin: 10
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                Layout.fillWidth: true
                visible: labelLexiconDirectory.text != ""
                Label {
                    text: qsTr("Lexicon Directory Path")
                }
                CustomLabel {
                    id: labelLexiconDirectory
                    Layout.fillWidth: true
                    elide: Text.ElideMiddle
                }
            }
        }
    }

    ListView {
        id: listViewEntries
        property alias listModelEntries: listModelEntries
        property int storedIndex: -1
        property var checkedListItems: []
        signal checkedItemsChanged();
        onCheckedItemsChanged: {
            //console.log(JSON.stringify(checkedListItems));
            labelCheckedItems.visible = checkedListItems.length > 0;
        }
        function processEntries(json) {
            var entries = {};
            var topicName = "";
            var topicId = 0;

            json.forEach(function(entry, index) {
                if (!entries.hasOwnProperty(entry.translation_key)) {
                    entries[entry.translation_key] = {};
                }

                topicName = entry.topic_name;
                topicId = entry.topic_id;

                entries[entry.translation_key][entry.language_name] = {
                    "entry_id": entry.id,
                    "translation_key": entry.translation_key,
                    "translation": entry.translation,
                    "language_name": entry.language_name,
                    "language_id": entry.language_id,
                    "topic_name": entry.topic_name,
                    "topic_id": entry.topic_id
                };
            });

            // add missing translations
            for (const [key, value] of Object.entries(entries)) {
                var foundLanguages = [];
                for (const languageName in value) {
                    foundLanguages.push(languageName);
                }

                if (window.languages.length > foundLanguages.length) {
                    window.languages.forEach(function(language, index) {
                        if (!foundLanguages.includes(language.name)) {
                            var topic = listModelTopics.get(comboBoxTopic.currentIndex);

                            value[language.name] = {
                                "entry_id": 0,
                                "translation_key": key,
                                "translation": "",
                                "language_name": language.name,
                                "language_id": language.id,
                                "topic_name": topicName,
                                "topic_id": topicId
                            };
                        }
                    });
                }
            }

            listModelEntries.clear();
            listViewEntries.checkedListItems = [];
            listViewEntries.checkedItemsChanged();
            for (const [key, value] of Object.entries(entries)) {
                var missingTranslationsCount = 0;

                for (const languageName in value) {
                    var translation = value[languageName];

                    if (translation.entry_id === 0) {
                        missingTranslationsCount++;
                    }
                }

                listModelEntries.append({
                                            "translation_key": key,
                                            "missing_translations_count": missingTranslationsCount,
                                            "translations": value
                                        });
            }

            if (listViewEntries.storedIndex >= 0) {
                listViewEntries.positionViewAtIndex(listViewEntries.storedIndex, ListView.Center);
            }
        }
        anchors.fill: parent
        ScrollBar.vertical: ScrollBar {
            id: scrollBarEntries
            width: 25
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
        model: ListModel {
            id: listModelEntries
        }
        delegate: CheckDelegate {
            id: item
            width: listViewEntries.width - scrollBarEntries.width
            height: itemDelegateRow.height
            RowLayout {
                id: itemDelegateRow
                width: parent.width - 20
                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.margins: 10
                    Text {
                        color: item.checked ? "#005bed" : Qt.black
                        text: model.translation_key
                    }
                    Text {
                        color: model.missing_translations_count > 0 ? "red" : "green"
                        text: model.missing_translations_count > 0 ? qsTr("%1 missing translation(s)").arg(model.missing_translations_count) : qsTr("complete")
                    }
                }
                RowLayout {
                    Layout.alignment: Qt.AlignRight
                    Layout.margins: 10
                    CustomButton {
                        id: buttonEditTranslations
                        text: qsTr("Edit")
                        onClicked: {
                            listViewEntries.storedIndex = model.index;

                            var translations = listModelEntries.get(model.index);

                            dialogEditTranslations.setup(translations);
                            dialogEditTranslations.open();
                        }
                    }
                    CustomButton {
                        id: buttonMoveTranslations
                        text: qsTr("Move")
                        onClicked: {
                            listViewEntries.storedIndex = model.index;

                            var translations = listModelEntries.get(model.index);

                            dialogMoveTranslations.setup(translations);
                            dialogMoveTranslations.open();
                        }
                    }
                    CustomButton {
                        text: qsTr("Delete")
                        onClicked: {
                            listViewEntries.storedIndex = model.index;

                            var translations = listModelEntries.get(model.index);

                            dialogDeleteTranslations.setup(translations);
                            dialogDeleteTranslations.open();
                        }
                    }
                }
            }
            onDoubleClicked: {
                buttonEditTranslations.clicked();
            }
            onCheckedChanged: {
                if (checked) {
                    if (listViewEntries.checkedListItems.indexOf(model.index) < 0) {
                        listViewEntries.checkedListItems.push(model.index);

                        listViewEntries.checkedListItems.sort((a, b) => {
                            return a - b;
                        });

                        listViewEntries.checkedItemsChanged();
                    }
                } else {
                    var uncheckedListItemsIndex = listViewEntries.checkedListItems.indexOf(model.index);

                    if (uncheckedListItemsIndex >= 0) {
                        listViewEntries.checkedListItems.splice(uncheckedListItemsIndex, 1);

                        listViewEntries.checkedListItems.sort((a, b) => {
                            return a - b;
                        });

                        listViewEntries.checkedItemsChanged();
                    }
                }
            }
        }
        Menu {
            id: listViewEntriesContextMenu
            MenuItem {
                text: qsTr("Move Selected Translations")
                onTriggered: {
                    var translations = [];

                    listViewEntries.checkedListItems.forEach((checkedListItemIndex, index) => {
                        var translation = listViewEntries.listModelEntries.get(checkedListItemIndex);
                        translations.push(translation);
                    });

                    dialogMoveSelectedTranslations.setup(translations);
                    dialogMoveSelectedTranslations.open();
                }
            }
            MenuItem {
                text: qsTr("Delete Selected Translations")
                onTriggered: {
                    var translations = [];

                    listViewEntries.checkedListItems.forEach((checkedListItemIndex, index) => {
                        var translation = listViewEntries.listModelEntries.get(checkedListItemIndex);
                        translations.push(translation);
                    });

                    dialogDeleteSelectedTranslations.setup(translations);
                    dialogDeleteSelectedTranslations.open();
                }
            }
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: (mouse) => {
                if (mouse.button === Qt.RightButton) {
                    listViewEntriesContextMenu.x = mouse.x;
                    listViewEntriesContextMenu.y = mouse.y;
                    listViewEntriesContextMenu.open();
                }
            }
        }
    }
}
