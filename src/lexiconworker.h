#ifndef LEXICONWORKER_H
#define LEXICONWORKER_H

#include <QObject>
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QByteArray>
#include <QDebug>
#include "databasecomponent.h"

class LexiconWorker : public QObject
{
    Q_OBJECT
    // constants:
    // attributes:
    DatabaseComponent* m_databaseComponent = nullptr;
    QString m_stringLiteralChar = "'";
    // methods:
    QString convertJsonToString(const QJsonObject &data);
    QString convertJsonToString(const QJsonArray &data);
    void processLexiconFile(int &languageId, int &topicId, QFile &file);
    void emitUpdatedDatabaseStats();
signals:
    void importFromLexiconDirectoryFinished();
    void exportAsFileStructureFinished();
    void saveTopicToFileStructureFinished();
    void updatedDatabaseStats(QString stats);
    void updatedProcessingFile(QString fileName);
    void getTopicsResult(QString topics);
    void getTopicEntriesResult(QString entries);
    void getLanguagesResult(QString languages);
    void searchEntriesResult(QString entries);
    void getMissingTranslationsResult(QString missingTranslations);
    void addTopicResult(int topicId);
    void addEntryResult(int entryId);
    void addLanguageResult(int languageId);
    void updateEntryResult(int entryId);
    void updateTopicResult(int topicId);
    void moveEntryResult(int entryId);
    void deleteEntryResult(int entryId);
    void deleteTopicResult(int topicId);
    void deleteLanguageResult(int languageId);
    void updatePackageResult(QString data);
    void updateSettingResult(QString data);
    void getSettingResult(QString setting);
    void isAlreadyUsedTranslationKeyResult(QString topics);
    void isAlreadyUsedTopicNameResult(QString data);
    void isAlreadyUsedLanguageNameResult(QString data);
public slots:
    void emptyDatabase();
    void importFromLexiconDirectory(QString path);
    void exportAsFileStructure(QString path);
    void saveTopicToFileStructure(int topicId, QString pathLexicon);
    void addLanguage(QString language);
    void addTopic(QString topic);
    void addEntry(int languageId,
                  int topicId,
                  QString translationKey,
                  QString translation);
    void updateEntry(int entryId,
                     QString translationKey,
                     QString translation);
    void updateTopic(int topicId,
                     QString name);
    void moveEntry(int entryId,
                   int topicId);
    void deleteEntry(int entryId);
    void deleteTopic(int topicId);
    void deleteLanguage(int languageId);
    void updatePackage(QString name, QString description, QString author, int date);
    void updateSetting(QString key, QString value);
    void getSetting(QString key);
    void getTopics();
    void getTopicEntries(int topicId);
    void getLanguages();
    void searchEntries(QString searchTerm, QString orderBy = "translation_key", QString sortOrder = "ASC");
    void getMissingTranslations();
    void isAlreadyUsedTranslationKey(QString translationKey);
    void isAlreadyUsedTopicName(QString topicName);
    void isAlreadyUsedLanguageName(QString languageName);
public:
    // constructor:
    LexiconWorker(QObject* parent = nullptr);
};

#endif // LEXICONWORKER_H
