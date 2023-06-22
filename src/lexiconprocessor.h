#ifndef LEXICONPROCESSOR_H
#define LEXICONPROCESSOR_H

#include <QObject>
#include <QThread>
#include <QDebug>
#include "lexiconworker.h"

class LexiconProcessor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
    // constants:
    const QString DATABASE_DRIVER = "QSQLITE";
    // attributes:
    bool m_loading = false;
    QThread m_workerThread;
    QString m_stringLiteralChar = "'";
    // methods:
signals:
    void loadingChanged();
    void emptyDatabase();
    void importFromLexiconDirectory(QString path);
    void importFromLexiconDirectoryFinished();
    void exportAsFileStructure(QString path);
    void exportAsFileStructureFinished();
    void saveTopicToFileStructure(int topicId, QString lexiconPath);
    void saveTopicToFileStructureFinished();
    void updatedDatabaseStats(QString stats);
    void updatedProcessingFile(QString fileName);
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
    void deleteEntry(int entryId);
    void deleteLanguage(int languageId);
    void deleteTopic(int topicId);
    void updatePackage(QString name, QString description, QString author, int date);
    void updateSetting(QString key, QString value);
    void getSettingResult(QString setting);
    void getTopics();
    void getTopicsResult(QString topics);
    void getTopicEntries(int topicId);
    void getTopicEntriesResult(QString entries);
    void getLanguages();
    void getLanguagesResult(QString languages);
    void searchEntries(QString searchTerm, QString orderBy = "translation_key", QString sortOrder = "ASC");
    void searchEntriesResult(QString entries);
    void getMissingTranslations();
    void getMissingTranslationsResult(QString missingTranslations);
    void addTopicResult(int topicId);
    void addEntryResult(int entryId);
    void addLanguageResult(int languageId);
    void updateEntryResult(int entryId);
    void updateTopicResult(int topicId);
    void moveEntry(int entryId, int topicId);
    void moveEntryResult(int entryId);
    void deleteEntryResult(int entryId);
    void deleteLanguageResult(int languageId);
    void deleteTopicResult(int topicId);
    void updatePackageResult(QString data);
    void updateSettingResult(QString data);
    void getSetting(QString key);
    void isAlreadyUsedTranslationKey(QString translationKey);
    void isAlreadyUsedTranslationKeyResult(QString topics);
    void isAlreadyUsedTopicName(QString topicName);
    void isAlreadyUsedTopicNameResult(QString data);
    void isAlreadyUsedLanguageName(QString languageName);
    void isAlreadyUsedLanguageNameResult(QString data);
public slots:
    QString getSettingsKey(int key);
public:
    // loading
    bool loading();
    void setLoading(const bool &loading);
    // constructor:
    LexiconProcessor(QObject* parent = nullptr);
    // enum
    enum Settings {
        SelectedLexiconDirectoryPath
    };
    Q_ENUM(Settings)
    // destructor
    ~LexiconProcessor();
};

#endif // LEXICONPROCESSOR_H
