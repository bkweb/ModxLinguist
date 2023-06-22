#ifndef DATABASECOMPONENT_H
#define DATABASECOMPONENT_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlField>
#include <QSqlRecord>
#include <QSqlResult>
#include <QStandardPaths>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QByteArray>
#include <QDebug>

class DatabaseComponent : public QObject
{
    Q_OBJECT
    // constants:
    const QString DATABASE_DRIVER = "QSQLITE";
    const QString DATABASE_FILE = "db-modx-linguist.db";
    const QString TABLE_LOG = "log";
    const QString QUERY_CREATE_LOG =
            "CREATE TABLE IF NOT EXISTS " + TABLE_LOG +
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "date DEFAULT CURRENT_TIMESTAMP NOT NULL,"
            "text TEXT)";
    const QString TABLE_SETTINGS = "settings";
    const QString QUERY_CREATE_SETTINGS =
        "CREATE TABLE IF NOT EXISTS " + TABLE_SETTINGS +
        "(key TEXT PRIMARY KEY NOT NULL,"
        "value TEXT)";
    const QString TABLE_PACKAGE = "package";
    const QString QUERY_CREATE_PACKAGE =
        "CREATE TABLE IF NOT EXISTS " + TABLE_PACKAGE +
        "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
        "name TEXT UNIQUE,"
        "description TEXT,"
        "author TEXT,"
        "date INTEGER)";
    const QString TABLE_LANGUAGE = "language";
    const QString QUERY_CREATE_LANGUAGE =
            "CREATE TABLE IF NOT EXISTS " + TABLE_LANGUAGE +
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT UNIQUE)";
    const QString TABLE_TOPIC = "topic";
    const QString QUERY_CREATE_TOPIC =
            "CREATE TABLE IF NOT EXISTS " + TABLE_TOPIC +
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "name TEXT UNIQUE)";
    const QString TABLE_ENTRY = "entry";
    const QString QUERY_CREATE_ENTRY =
            "CREATE TABLE IF NOT EXISTS " + TABLE_ENTRY +
            "(id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL,"
            "language_id INTEGER,"
            "topic_id INTEGER,"
            "translation_key TEXT,"
            "translation TEXT)";
    // attributes:
    QSqlDatabase m_databaseConnection;
    bool m_storeOnlyInMemory = false;
    // methods:
    void setUpDatabase();
    QString convertJsonToString(const QJsonObject &data);
    void outputSqlError(const QSqlQuery &query);
    int getCount(QString tableName);
    int getEntryId(int languageId,
                   int topicId,
                   QString translationKey,
                   QString translation);
    int getEntryId(int languageId,
                   int topicId,
                   QString translationKey);
signals:
    void databaseConnectionError(QString errorMessage);
    void logJsonDataResult(bool result, QString data);
    void addLanguageResult(bool result, QString data);
    void addTopicResult(bool result, QString data);
    void addEntryResult(bool result, QString data);
    void updateEntryResult(bool result, QString data);
    void deleteEntryResult(bool result, QString data);
    void deleteLanguageResult(bool result, QString data);
    void deleteTopicResult(bool result, QString data);
    void updatePackageResult(bool result, QString data);
    void updateSettingResult(bool result, QString data);
    void updateTopicResult(bool result, QString data);
    void moveEntryResult(bool result, QString data);
    void setDatabaseLocation(QString message);
    void deleteSqliteDatabaseResult(bool result);
public slots:
    void deleteSqliteDatabase();
    void logJsonData(const QString &data);
    void updatePackage(const QString &name,
                       const QString &description,
                       const QString &author,
                       const int &date);
    void updateSetting(const QString &key,
                       const QString &value);
    int addLanguage(const QString &name);
    int addTopic(const QString &name);
    int addEntry(const int &languageId,
                 const int &topicId,
                 const QString &translationKey,
                 const QString &translation);
    int updateEntry(const int &entryId,
                    const QString &translationKey,
                    const QString &translation);
    int updateTopic(const int &topicId,
                    const QString &name);
    int moveEntry(const int &entryId,
                  const int &topicId);
    bool deleteEntry(const int &entryId);
    bool deleteLanguage(const int &languageId);
    bool deleteTopic(const int &topicId);
    void query(const QString &queryString);
    void emptyDatabase();
    QStringList getTables();
    QStringList getColumns(const QString &tableName);
    QVariantList getRows(const QString &tableName,
                         const QStringList &selectFields = QStringList("*"),
                         const int &offset = 0,
                         const int &limit = 0,
                         const QString &orderBy = "id",
                         const QString &sortOrder = "DESC");
    QVariantList getAllRows(const QString &tableName,
                            const QStringList &selectFields = QStringList("*"),
                            const QString &orderBy = "id",
                            const QString &sortOrder = "DESC");
    QJsonObject getRowById(const QString &tableName,
                           const int &id);
    QVariantList getTopicEntries(const int &topicId);
    QVariantList getTopicEntries(const int &languageId,
                                 const int &topicId);
    QVariantList searchEntries(const QString &searchTerm,
                               const QString &orderBy = "translation_key",
                               const QString &sortOrder = "ASC");
    QVariantList getMissingTranslations();
    QVariantList getLanguages();
    QJsonObject getPackage();
    QJsonObject getSetting(QString key);
    int getLanguageCount();
    int getTopicCount();
    int getEntryCount();
    int getMissingTranslationCount();
    int getLanguageId(QString language);
    int getTopicId(QString topic);
    QVariantList isAlreadyUsedTranslationKey(QString translationKey);
    bool isAlreadyUsedTopicName(QString topicName);
    bool isAlreadyUsedLanguageName(QString languageName);
    void deleteDatabase();
public:
    QString getTableSettings();
    QString getTableLanguage();
    QString getTableTopic();
    QString getTableEntry();
    QString getTablePackage();
    // constructor:
    DatabaseComponent(QObject* parent = nullptr);
};

#endif // DATABASECOMPONENT_H
