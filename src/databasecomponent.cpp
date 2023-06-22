#include "databasecomponent.h"

DatabaseComponent::DatabaseComponent(QObject* parent) : QObject(parent) {
    if (!QSqlDatabase::isDriverAvailable(DATABASE_DRIVER)) {
        qDebug() << "database driver not available:" << DATABASE_DRIVER;
        emit databaseConnectionError("database driver not available: " + DATABASE_DRIVER);
    }

    setUpDatabase();
}

void DatabaseComponent::setUpDatabase() {
    m_databaseConnection = QSqlDatabase::addDatabase(DATABASE_DRIVER);

    if (m_storeOnlyInMemory) {
        m_databaseConnection.setDatabaseName(":memory:");
        emit setDatabaseLocation("database stored only in memory");
    } else {
        m_databaseConnection.setDatabaseName(DATABASE_FILE);
        emit setDatabaseLocation(QString("database file: %1").arg(DATABASE_FILE));
    }

    if (!m_databaseConnection.open()) {
        qWarning() << "ERROR:" << m_databaseConnection.lastError().text();
        emit databaseConnectionError(QString("database error: %1").arg(m_databaseConnection.lastError().text()));
    }

    // set up database tables
    query(QUERY_CREATE_LOG);
    query(QUERY_CREATE_SETTINGS);
    query(QUERY_CREATE_PACKAGE);
    query(QUERY_CREATE_LANGUAGE);
    query(QUERY_CREATE_TOPIC);
    query(QUERY_CREATE_ENTRY);
}

void DatabaseComponent::deleteSqliteDatabase() {
    if (m_databaseConnection.isOpen()) {
        m_databaseConnection.close();
    }

    QFile file(DATABASE_FILE);
    bool result = file.remove();
    if (!result) {
        qDebug() << "Deleting the SQLite-Database failed:" << file.errorString();
    }

    setUpDatabase();

    emit deleteSqliteDatabaseResult(result);
}

QString DatabaseComponent::convertJsonToString(const QJsonObject &data) {
    QJsonDocument doc(data);
    return QString(doc.toJson(QJsonDocument::Indented));
}

void DatabaseComponent::outputSqlError(const QSqlQuery &query) {
    qDebug() << "SQLite query failed" << "(" << query.lastError().nativeErrorCode() << "):" << query.lastError().text() << ":" << query.executedQuery();
}

void DatabaseComponent::logJsonData(const QString &data) {
    if (data.isEmpty() || data.isNull()) {
        qDebug() << "no data to log";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO " + TABLE_LOG + " (text) VALUES (:text)");
    query.bindValue(":text", data);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    emit logJsonDataResult(result, data);
}

void DatabaseComponent::updatePackage(const QString &name, const QString &description, const QString &author, const int &date) {
    if (name.isEmpty() || name.isNull()) {
        qDebug() << "no valid data to update the package data";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO " + TABLE_PACKAGE + "("
                                                     "id,"
                                                     "name,"
                                                     "description,"
                                                     "author,"
                                                     "date)"
                                                     " VALUES ("
                                                     "1,"
                                                     ":name,"
                                                     ":description,"
                                                     ":author,"
                                                     ":date)");
    query.bindValue(":name", name);
    query.bindValue(":description", description);
    query.bindValue(":author", author);
    query.bindValue(":date", date);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(query.lastInsertId().toInt()));
    data.insert("name", name);
    data.insert("description", description);
    data.insert("author", author);
    data.insert("date", date);

    emit updatePackageResult(result, convertJsonToString(data));
}

//
void DatabaseComponent::updateSetting(const QString &key, const QString &value) {
    if (key.isEmpty() || key.isNull()) {
        qDebug() << "no valid data to update the settings data";
        return;
    }

    QSqlQuery query;
    query.prepare("INSERT OR REPLACE INTO " + TABLE_SETTINGS + "("
                                                              "key,"
                                                              "value)"
                                                              " VALUES ("
                                                              ":key,"
                                                              ":value)");
    query.bindValue(":key", key);
    query.bindValue(":value", value);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("key", QJsonValue(key));
    data.insert("value", value);

    emit updatePackageResult(result, convertJsonToString(data));
}

int DatabaseComponent::addLanguage(const QString &name) {
    if (name.isEmpty() || name.isNull() || isAlreadyUsedLanguageName(name)) {
        qDebug() << "no valid data to add to the languages";
        return -1;
    }

    QSqlQuery query;
    query.prepare("REPLACE INTO " + TABLE_LANGUAGE + "("
                  "name)"
                  " VALUES ("
                  ":name)");
    query.bindValue(":name", name);
    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(query.lastInsertId().toInt()));
    data.insert("name", name);

    emit addLanguageResult(result, convertJsonToString(data));

    return query.lastInsertId().toInt();
}

int DatabaseComponent::addTopic(const QString &name) {
    if (name.isEmpty() || name.isNull() || isAlreadyUsedTopicName(name)) {
        qDebug() << "no valid data to add to the topics";
        return -1;
    }

    QSqlQuery query;
    query.prepare("REPLACE INTO " + TABLE_TOPIC + "("
                  "name)"
                  " VALUES ("
                  ":name)");
    query.bindValue(":name", name);
    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(query.lastInsertId().toInt()));
    data.insert("name", name);

    emit addTopicResult(result, convertJsonToString(data));

    return query.lastInsertId().toInt();
}

int DatabaseComponent::addEntry(const int &languageId,
                                const int &topicId,
                                const QString &translationKey,
                                const QString &translation) {
    if (languageId <= 0
     || topicId <= 0
     || translationKey.isEmpty() || translationKey.isNull()
     || translation.isEmpty() || translation.isNull()) {
        qDebug() << "no valid data to add to the entries";
        return -1;
    }

    // check if a row does already contain the specified data
    int entryId = getEntryId(languageId, topicId, translationKey, translation);
    if (entryId > 0) {
        return entryId;
    }

    QSqlQuery query;
    query.prepare("REPLACE INTO " + TABLE_ENTRY + "("
                  "language_id,"
                  "topic_id,"
                  "translation_key,"
                  "translation)"
                  " VALUES ("
                  ":language_id,"
                  ":topic_id,"
                  ":translation_key,"
                  ":translation)");
    query.bindValue(":language_id", languageId);
    query.bindValue(":topic_id", topicId);
    query.bindValue(":translation_key", translationKey);
    query.bindValue(":translation", translation);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(query.lastInsertId().toInt()));
    data.insert("language_id", languageId);
    data.insert("topic_id", topicId);
    data.insert("translation_key", QJsonValue(translationKey));
    data.insert("translation", QJsonValue(translation));

    emit addEntryResult(result, convertJsonToString(data));

    return query.lastInsertId().toInt();
}

int DatabaseComponent::updateEntry(const int &entryId,
                                   const QString &translationKey,
                                   const QString &translation) {
    if (entryId <= 0
     || translationKey.isEmpty() || translationKey.isNull()
     || translation.isEmpty() || translation.isNull()) {
        qDebug() << "no valid data to update an entry";
        return -1;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + TABLE_ENTRY + " SET translation_key = :translation_key, translation = :translation WHERE id = :entry_id");
    query.bindValue(":entry_id", entryId);
    query.bindValue(":translation_key", translationKey);
    query.bindValue(":translation", translation);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(entryId));
    data.insert("translation_key", QJsonValue(translationKey));
    data.insert("translation", QJsonValue(translation));

    emit updateEntryResult(result, convertJsonToString(data));

    return entryId;
}

int DatabaseComponent::updateTopic(const int &topicId,
                                   const QString &name) {
    if (topicId <= 0 || isAlreadyUsedTopicName(name)
        || name.isEmpty() || name.isNull()) {
        qDebug() << "no valid data to update a topic";
        return -1;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + TABLE_TOPIC + " SET name = :name WHERE id = :topic_id");
    query.bindValue(":topic_id", topicId);
    query.bindValue(":name", name);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(topicId));
    data.insert("name", QJsonValue(name));

    emit updateTopicResult(result, convertJsonToString(data));

    return topicId;
}

int DatabaseComponent::moveEntry(const int &entryId,
                                 const int &topicId) {
    if (topicId <= 0
        || entryId <= 0) {
        qDebug() << "no valid data for moving entry";
        return -1;
    }

    QSqlQuery query;
    query.prepare("UPDATE " + TABLE_ENTRY + " SET topic_id = :topic_id WHERE id = :entry_id");
    query.bindValue(":topic_id", topicId);
    query.bindValue(":entry_id", entryId);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("topicId", QJsonValue(topicId));
    data.insert("entryId", QJsonValue(entryId));

    emit moveEntryResult(result, convertJsonToString(data));

    return query.numRowsAffected();
}

bool DatabaseComponent::deleteEntry(const int &entryId) {
    if (entryId <= 0) {
        qDebug() << "no valid data to delete from entry table";
        return false;
    }

    QSqlQuery query;
    query.prepare("DELETE FROM " + TABLE_ENTRY + " WHERE id = :entry_id");
    query.bindValue(":entry_id", entryId);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(entryId));

    emit deleteEntryResult(result, convertJsonToString(data));

    return true;
}

bool DatabaseComponent::deleteLanguage(const int &languageId) {
    if (languageId <= 0) {
        qDebug() << "no valid data to delete from language table";
        return false;
    }

    // delete related translation entries
    QSqlQuery query;
    query.prepare("DELETE FROM " + TABLE_ENTRY + " WHERE language_id = :language_id");
    query.bindValue(":language_id", languageId);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    // delete language
    query = QSqlQuery();
    query.prepare("DELETE FROM " + TABLE_LANGUAGE + " WHERE id = :language_id");
    query.bindValue(":language_id", languageId);

    result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(languageId));

    emit deleteLanguageResult(result, convertJsonToString(data));

    return true;
}

bool DatabaseComponent::deleteTopic(const int &topicId) {
    if (topicId <= 0) {
        qDebug() << "no valid data to delete from topic table";
        return false;
    }

    // delete related translation entries
    QSqlQuery query;
    query.prepare("DELETE FROM " + TABLE_ENTRY + " WHERE topic_id = :topic_id");
    query.bindValue(":topic_id", topicId);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    // delete topic
    query = QSqlQuery();
    query.prepare("DELETE FROM " + TABLE_TOPIC + " WHERE id = :topic_id");
    query.bindValue(":topic_id", topicId);

    result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    QJsonObject data;
    data.insert("id", QJsonValue(topicId));

    emit deleteTopicResult(result, convertJsonToString(data));

    return true;
}

int DatabaseComponent::getCount(QString tableName) {
    if (tableName.isEmpty() || tableName.isNull()) {
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT COUNT(id) AS count FROM " + tableName);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.first();
        QVariant value = query.value("count");

        return value.toInt();
    }

    return -1;
}

int DatabaseComponent::getLanguageCount() {
    return getCount(TABLE_LANGUAGE);
}

int DatabaseComponent::getTopicCount() {
    return getCount(TABLE_TOPIC);
}

int DatabaseComponent::getEntryCount() {
    return getCount(TABLE_ENTRY);
}

int DatabaseComponent::getMissingTranslationCount() {
    QSqlQuery query;
    query.prepare("SELECT COUNT(DISTINCT translation_key) AS count, ("
                  "     SELECT COUNT(id) "
                  "     FROM " + TABLE_ENTRY + " B "
                  "     WHERE B.translation_key = A.translation_key"
                  ") AS translation_count "
                  "FROM " + TABLE_ENTRY + " A "
                  "WHERE translation_count < ("
                  "     SELECT COUNT(id) "
                  "     FROM " + TABLE_LANGUAGE + ""
                  ")");

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.first();
        QVariant value = query.value("count");

        return value.toInt();
    }

    return -1;
}

QVariantList DatabaseComponent::isAlreadyUsedTranslationKey(QString translationKey) {
    if (translationKey.isEmpty() || translationKey.isNull()) {
        return QVariantList();
    }

    QJsonArray rows;

    QSqlQuery query;
    query.prepare("SELECT DISTINCT " + TABLE_TOPIC + ".* "
                  "FROM " + TABLE_TOPIC + " "
                  "INNER JOIN " + TABLE_ENTRY + " "
                  "ON (" + TABLE_ENTRY + ".topic_id = " + TABLE_TOPIC + ".id) "
                  "WHERE " + TABLE_ENTRY + ".translation_key = :translation_key");
    query.bindValue(":translation_key", translationKey);

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

bool DatabaseComponent::isAlreadyUsedTopicName(QString topicName) {
    if (topicName.isEmpty() || topicName.isNull()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_TOPIC + " WHERE name = :name");
    query.bindValue(":name", topicName);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return true;
        }
    }

    return false;
}

bool DatabaseComponent::isAlreadyUsedLanguageName(QString languageName) {
    if (languageName.isEmpty() || languageName.isNull()) {
        return false;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_LANGUAGE + " WHERE name = :name");
    query.bindValue(":name", languageName);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return true;
        }
    }

    return false;
}

int DatabaseComponent::getLanguageId(QString language) {
    if (language.isEmpty() || language.isNull()) {
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_LANGUAGE + " WHERE name = :name");
    query.bindValue(":name", language);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return value.toInt();
        }
    }

    return -1;
}

int DatabaseComponent::getTopicId(QString topic) {
    if (topic.isEmpty() || topic.isNull()) {
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_TOPIC + " WHERE name = :name");
    query.bindValue(":name", topic);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return value.toInt();
        }
    }

    return -1;
}

int DatabaseComponent::getEntryId(int languageId, int topicId, QString translationKey, QString translation) {
    if (languageId <= 0
     || topicId <= 0
     || translationKey.isEmpty() || translationKey.isNull()
     || translation.isEmpty() || translation.isNull()) {
        qDebug() << "no valid data to get from the entries";
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_ENTRY + " WHERE language_id = :language_id AND topic_id = :topic_id AND translation_key = :translation_key AND translation = :translation");
    query.bindValue(":language_id", languageId);
    query.bindValue(":topic_id", topicId);
    query.bindValue(":translation_key", translationKey);
    query.bindValue(":translation", translation);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return value.toInt();
        }
    }

    return -1;
}

int DatabaseComponent::getEntryId(int languageId, int topicId, QString translationKey) {
    if (languageId <= 0
        || topicId <= 0
        || translationKey.isEmpty() || translationKey.isNull()) {
        qDebug() << "no valid data to get from the entries";
        return -1;
    }

    QSqlQuery query;
    query.prepare("SELECT id FROM " + TABLE_ENTRY + " WHERE language_id = :language_id AND topic_id = :topic_id AND translation_key = :translation_key");
    query.bindValue(":language_id", languageId);
    query.bindValue(":topic_id", topicId);
    query.bindValue(":translation_key", translationKey);

    bool result = query.exec();
    if (!result) {
        outputSqlError(query);
    }

    if (result) {
        query.last();
        int rowCount = query.at() + 1;

        if (rowCount > 0) {
            QVariant value = query.value("id");

            return value.toInt();
        }
    }

    return -1;
}

QStringList DatabaseComponent::getColumns(const QString &tableName) {
    if (tableName.isEmpty() || tableName.isNull()) {
        qDebug() << "no table name specified";
        return QStringList();
    }

    QStringList columns;

    QSqlRecord record = m_databaseConnection.record(tableName);
    for (int i = 0; i < record.count(); i++) {
        columns << record.field(i).name();
    }

    return columns;
}

void DatabaseComponent::emptyDatabase() {
    QStringList tableNames = QStringList() << TABLE_PACKAGE << TABLE_LANGUAGE << TABLE_TOPIC << TABLE_ENTRY << TABLE_LOG;
    for (int i = 0; i < tableNames.count(); i++) {
        QString tableName = tableNames.at(i);

        query("DELETE FROM " + tableName);
        query("UPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = '" + tableName + "'");
    }
}

QStringList DatabaseComponent::getTables() {
    return m_databaseConnection.tables();
}

QVariantList DatabaseComponent::getAllRows(const QString &tableName,
                                           const QStringList &selectFields,
                                           const QString &orderBy,
                                           const QString &sortOrder) {
    if (tableName.isEmpty() || tableName.isNull()) {
        qDebug() << "no table name specified";
        return QVariantList();
    } else if (selectFields.count() == 0) {
        qDebug() << "no fields selected";
        return QVariantList();
    }

    QJsonArray rows;

    QSqlQuery query;
    QString queryString = QString("SELECT %4 FROM %1 ORDER BY %2 %3").arg(tableName).arg(orderBy).arg(sortOrder).arg(selectFields.join(","));
    query.prepare(queryString);

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

QVariantList DatabaseComponent::getTopicEntries(const int &topicId) {
    if (topicId <= 0) {
        qDebug() << "no valid topicId specified";
        return QVariantList();
    }

    QJsonArray rows;

    QSqlQuery query;
    query.prepare("SELECT " + TABLE_ENTRY + ".*, " + TABLE_LANGUAGE + ".name AS language_name, " + TABLE_TOPIC + ".name AS topic_name FROM " + TABLE_ENTRY + " INNER JOIN " + TABLE_TOPIC + " ON (topic.id = topic_id) INNER JOIN language ON (language.id = language_id) WHERE topic_id = :topic_id ORDER BY translation_key ASC, language_id ASC");
    query.bindValue(":topic_id", topicId);

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

QVariantList DatabaseComponent::getTopicEntries(const int &languageId, const int &topicId) {
    if (languageId <= 0 || topicId <= 0) {
        qDebug() << "no valid languageId or topicId specified";
        return QVariantList();
    }

    QJsonArray rows;

    QSqlQuery query;
    query.prepare("SELECT " + TABLE_ENTRY + ".* "
                  "FROM " + TABLE_ENTRY + " "
                  "WHERE " + TABLE_ENTRY + ".topic_id = :topic_id "
                  "AND " + TABLE_ENTRY + ".language_id = :language_id "
                  "ORDER BY " + TABLE_ENTRY + ".translation_key ASC");
    query.bindValue(":language_id", languageId);
    query.bindValue(":topic_id", topicId);

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

QVariantList DatabaseComponent::searchEntries(const QString &searchTerm,
                                              const QString &orderBy,
                                              const QString &sortOrder) {
    QJsonArray rows;

    QSqlQuery query;
    query.prepare("SELECT " + TABLE_ENTRY + ".*, " + TABLE_LANGUAGE + ".name AS language_name, " + TABLE_TOPIC + ".name AS topic_name FROM " + TABLE_ENTRY + " INNER JOIN " + TABLE_TOPIC + " ON (topic.id = topic_id) INNER JOIN " + TABLE_LANGUAGE + " ON (language.id = language_id) WHERE translation_key LIKE :translation_key ORDER BY " + orderBy + " " + sortOrder + ", language_id ASC");
    query.bindValue(":translation_key", "%" + searchTerm + "%");

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

QVariantList DatabaseComponent::getMissingTranslations() {
    QJsonArray rows;

    QSqlQuery query;
    query.prepare("SELECT A.*, " + TABLE_LANGUAGE + ".name AS language_name, " + TABLE_TOPIC + ".name AS topic_name "
                  "FROM " + TABLE_ENTRY + " A "
                  "INNER JOIN " + TABLE_LANGUAGE + " ON (" + TABLE_LANGUAGE + ".id = A.language_id) "
                  "INNER JOIN " + TABLE_TOPIC + " ON (" + TABLE_TOPIC + ".id = A.topic_id) "
                  "WHERE A.translation_key IN("
                  "     SELECT DISTINCT B.translation_key "
                  "     FROM " + TABLE_ENTRY + " B "
                  "     WHERE ("
                  "         SELECT COUNT(id) "
                  "         FROM " + TABLE_ENTRY + " C "
                  "         WHERE C.translation_key = B.translation_key"
                  "     ) < ("
                  "         SELECT COUNT(id) "
                  "         FROM " + TABLE_LANGUAGE + ""
                  "     )"
                  ")");

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

QVariantList DatabaseComponent::getLanguages() {
    return getAllRows(TABLE_LANGUAGE, QStringList("*"), "name", "ASC");
}

QJsonObject DatabaseComponent::getPackage() {
    QSqlQuery query;
    query.prepare("SELECT * FROM " + TABLE_PACKAGE + " WHERE id = 1");

    QJsonObject row;
    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            return row;
        }
    }

    return row;
}

QJsonObject DatabaseComponent::getSetting(QString key) {
    if (key.isEmpty() || key.isNull()) {
        qDebug() << "no settings key specified";
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM " + TABLE_SETTINGS + " WHERE key = :key");
    query.bindValue(":key", key);

    QJsonObject row;
    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            return row;
        }
    }

    return row;
}

QJsonObject DatabaseComponent::getRowById(const QString &tableName, const int &id) {
    if (tableName.isEmpty() || tableName.isNull() || id <= 0) {
        qDebug() << "no table row specified";
        return QJsonObject();
    }

    QSqlQuery query;
    query.prepare("SELECT * FROM " + tableName + " WHERE id = :id");
    query.bindValue(":id", id);

    QJsonObject row;
    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            return row;
        }
    }

    return row;
}

QVariantList DatabaseComponent::getRows(const QString &tableName,
                                        const QStringList &selectFields,
                                        const int &offset,
                                        const int &limit,
                                        const QString &orderBy,
                                        const QString &sortOrder) {
    if (tableName.isEmpty() || tableName.isNull()) {
        qDebug() << "no table name specified";
        return QVariantList();
    } else if (selectFields.count() == 0) {
        qDebug() << "no fields selected";
        return QVariantList();
    } else if (offset < 0) {
        qDebug() << "invalid offset";
        return QVariantList();
    } else if (limit <= 0) {
        qDebug() << "invalid limit";
        return QVariantList();
    }

    QJsonArray rows;

    QSqlQuery query;
    QString queryString = QString("SELECT %6 FROM %1 ORDER BY %4 %5 LIMIT %2 OFFSET %3").arg(tableName).arg(limit).arg(offset).arg(orderBy).arg(sortOrder).arg(selectFields.join(","));
    query.prepare(queryString);

    if (query.exec()) {
        while (query.next()) {
            QSqlRecord result = query.record();
            QJsonObject row;
            for (int i = 0; i < result.count(); i++) {
                row.insert(result.fieldName(i), result.value(i).toJsonValue());
            }
            rows.append(row);
        }
    }

    return rows.toVariantList();
}

void DatabaseComponent::query(const QString &queryString) {
    if (queryString.isEmpty() || queryString.isNull()) {
        qDebug() << "no query string specified";
        return;
    }

    QSqlQuery query(queryString);
    if (!query.isActive()) {
        qDebug() << QString("ERROR: %1, query string: %2").arg(query.lastError().text()).arg(queryString);
    }
}

void DatabaseComponent::deleteDatabase() {
    if (QFile::exists(DATABASE_FILE)) {
        if (QFile::remove(DATABASE_FILE)) {
            qDebug() << "database deleted";
        }
    }
}

QString DatabaseComponent::getTableSettings() {
    return TABLE_SETTINGS;
}

QString DatabaseComponent::getTableLanguage() {
    return TABLE_LANGUAGE;
}

QString DatabaseComponent::getTableTopic() {
    return TABLE_TOPIC;
}

QString DatabaseComponent::getTableEntry() {
    return TABLE_ENTRY;
}

QString DatabaseComponent::getTablePackage() {
    return TABLE_PACKAGE;
}
