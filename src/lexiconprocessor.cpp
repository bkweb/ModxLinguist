#include "lexiconprocessor.h"

LexiconProcessor::LexiconProcessor(QObject* parent) : QObject(parent) {
    LexiconWorker* worker = new LexiconWorker;
    worker->moveToThread(&m_workerThread);

    connect(&m_workerThread, &QThread::finished, worker, &QObject::deleteLater);

    // import from lexicon directory
    connect(this, &LexiconProcessor::importFromLexiconDirectory, this, [=](QString path) {
        setLoading(true);
    });
    connect(this, &LexiconProcessor::importFromLexiconDirectory, worker, &LexiconWorker::importFromLexiconDirectory);

    // finished importing from lexicon directory
    connect(worker, &LexiconWorker::importFromLexiconDirectoryFinished, this, [=]() {
        setLoading(false);
        emit importFromLexiconDirectoryFinished();
    });

    // export as file structure
    connect(this, &LexiconProcessor::exportAsFileStructure, this, [=](QString path) {
        setLoading(true);
    });
    connect(this, &LexiconProcessor::exportAsFileStructure, worker, &LexiconWorker::exportAsFileStructure);

    // finished exporting as file structure
    connect(worker, &LexiconWorker::exportAsFileStructureFinished, this, [=]() {
        setLoading(false);
        emit exportAsFileStructureFinished();
    });

    // save topic to file structure
    connect(this, &LexiconProcessor::saveTopicToFileStructure, this, [=](int topicId, QString lexiconPath) {
        setLoading(true);
    });
    connect(this, &LexiconProcessor::saveTopicToFileStructure, worker, &LexiconWorker::saveTopicToFileStructure);

    // finished save topic to file structure
    connect(worker, &LexiconWorker::saveTopicToFileStructureFinished, this, [=]() {
        setLoading(false);
        emit saveTopicToFileStructureFinished();
    });

    // add language
    connect(this, &LexiconProcessor::addLanguage, worker, &LexiconWorker::addLanguage);
    connect(worker, &LexiconWorker::addLanguageResult, this, [=](int languageId) {
        emit addLanguageResult(languageId);
    });

    // add topic
    connect(this, &LexiconProcessor::addTopic, worker, &LexiconWorker::addTopic);
    connect(worker, &LexiconWorker::addTopicResult, this, [=](int topicId) {
        emit addTopicResult(topicId);
    });

    // updated database stats - sends language count, topic count and entry count
    connect(worker, &LexiconWorker::updatedDatabaseStats, this, [=](QString stats) {
        emit updatedDatabaseStats(stats);
    });

    // updated processing file - sends the name of the file being currently processed
    connect(worker, &LexiconWorker::updatedProcessingFile, this, [=](QString fileName) {
        emit updatedProcessingFile(fileName);
    });

    // get topics
    connect(this, SIGNAL(getTopics()), worker, SLOT(getTopics()));
    connect(worker, &LexiconWorker::getTopicsResult, this, [=](QString topics) {
        emit getTopicsResult(topics);
    });

    // get entries
    connect(this, SIGNAL(getTopicEntries(int)), worker, SLOT(getTopicEntries(int)));
    connect(worker, &LexiconWorker::getTopicEntriesResult, this, [=](QString entries) {
        emit getTopicEntriesResult(entries);
    });

    // get languages
    connect(this, SIGNAL(getLanguages()), worker, SLOT(getLanguages()));
    connect(worker, &LexiconWorker::getLanguagesResult, this, [=](QString languages) {
        emit getLanguagesResult(languages);
    });

    // search entries
    connect(this, SIGNAL(searchEntries(QString,QString,QString)), worker, SLOT(searchEntries(QString,QString,QString)));
    connect(worker, &LexiconWorker::searchEntriesResult, this, [=](QString entries) {
        emit searchEntriesResult(entries);
    });

    // get missing translations
    connect(this, SIGNAL(getMissingTranslations()), worker, SLOT(getMissingTranslations()));
    connect(worker, &LexiconWorker::getMissingTranslationsResult, this, [=](QString missingTranslations) {
        emit getMissingTranslationsResult(missingTranslations);
    });

    // add entry
    connect(this, SIGNAL(addEntry(int,int,QString,QString)), worker, SLOT(addEntry(int,int,QString,QString)));
    connect(worker, &LexiconWorker::addEntryResult, this, [=](int entryId) {
        emit addEntryResult(entryId);
    });

    // update entry
    connect(this, SIGNAL(updateEntry(int,QString,QString)), worker, SLOT(updateEntry(int,QString,QString)));
    connect(worker, &LexiconWorker::updateEntryResult, this, [=](int entryId) {
        emit updateEntryResult(entryId);
    });

    // move entry
    connect(this, SIGNAL(moveEntry(int,int)), worker, SLOT(moveEntry(int,int)));
    connect(worker, &LexiconWorker::moveEntryResult, this, [=](int entryId) {
        emit moveEntryResult(entryId);
    });

    // update topic
    connect(this, SIGNAL(updateTopic(int,QString)), worker, SLOT(updateTopic(int,QString)));
    connect(worker, &LexiconWorker::updateTopicResult, this, [=](int topicId) {
        emit updateTopicResult(topicId);
    });

    // delete entry
    connect(this, SIGNAL(deleteEntry(int)), worker, SLOT(deleteEntry(int)));
    connect(worker, &LexiconWorker::deleteEntryResult, this, [=](int entryId) {
        emit deleteEntryResult(entryId);
    });

    // delete language
    connect(this, SIGNAL(deleteLanguage(int)), worker, SLOT(deleteLanguage(int)));
    connect(worker, &LexiconWorker::deleteLanguageResult, this, [=](int languageId) {
        emit deleteLanguageResult(languageId);
    });

    // delete topic
    connect(this, SIGNAL(deleteTopic(int)), worker, SLOT(deleteTopic(int)));
    connect(worker, &LexiconWorker::deleteTopicResult, this, [=](int topicId) {
        emit deleteTopicResult(topicId);
    });

    // update package
    connect(this, SIGNAL(updatePackage(QString,QString,QString,int)), worker, SLOT(updatePackage(QString,QString,QString,int)));
    connect(worker, &LexiconWorker::updatePackageResult, this, [=](QString data) {
        emit updatePackageResult(data);
    });

    // update setting
    connect(this, SIGNAL(updateSetting(QString,QString)), worker, SLOT(updateSetting(QString,QString)));
    connect(worker, &LexiconWorker::updateSettingResult, this, [=](QString data) {
        emit updateSettingResult(data);
    });

    // get setting
    connect(this, SIGNAL(getSetting(QString)), worker, SLOT(getSetting(QString)));
    connect(worker, &LexiconWorker::getSettingResult, this, [=](QString setting) {
        emit getSettingResult(setting);
    });

    // is already used translation key
    connect(this, SIGNAL(isAlreadyUsedTranslationKey(QString)), worker, SLOT(isAlreadyUsedTranslationKey(QString)));
    connect(worker, &LexiconWorker::isAlreadyUsedTranslationKeyResult, this, [=](QString topics) {
        emit isAlreadyUsedTranslationKeyResult(topics);
    });

    // is already used topic name
    connect(this, SIGNAL(isAlreadyUsedTopicName(QString)), worker, SLOT(isAlreadyUsedTopicName(QString)));
    connect(worker, &LexiconWorker::isAlreadyUsedTopicNameResult, this, [=](QString data) {
        emit isAlreadyUsedTopicNameResult(data);
    });

    // is already used language name
    connect(this, SIGNAL(isAlreadyUsedLanguageName(QString)), worker, SLOT(isAlreadyUsedLanguageName(QString)));
    connect(worker, &LexiconWorker::isAlreadyUsedLanguageNameResult, this, [=](QString data) {
        emit isAlreadyUsedLanguageNameResult(data);
    });

    connect(this, SIGNAL(emptyDatabase()), worker, SLOT(emptyDatabase()));

    m_workerThread.start();
}

LexiconProcessor::~LexiconProcessor() {
    m_workerThread.quit();
    m_workerThread.wait();
}

bool LexiconProcessor::loading() {
    return m_loading;
}

void LexiconProcessor::setLoading(const bool &loading) {
    if (loading != m_loading) {
        m_loading = loading;
        emit loadingChanged();
    }
}

QString LexiconProcessor::getSettingsKey(int key) {
    switch (key) {
        case Settings::SelectedLexiconDirectoryPath: return "selected_lexicon_directory_path";
        default: return "";
    };

    return "";
}
