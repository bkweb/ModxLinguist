#include "lexiconworker.h"

LexiconWorker::LexiconWorker(QObject* parent) : QObject(parent) {
    ianaLanguages = {"aa", "ab", "ae", "af", "ak", "am", "an", "ar", "as", "av", "ay", "az", "ba", "be", "bg", "bh", "bi", "bm", "bn", "bo", "br", "bs", "ca", "ce", "ch", "co", "cr", "cs", "cu", "cv", "cy", "da", "de", "dv", "dz", "ee", "el", "en", "eo", "es", "et", "eu", "fa", "ff", "fi", "fj", "fo", "fr", "fy", "ga", "gd", "gl", "gn", "gu", "gv", "ha", "he", "hi", "ho", "hr", "ht", "hu", "hy", "hz", "ia", "id", "ie", "ig", "ii", "ik", "in", "io", "is", "it", "iu", "iw", "ja", "ji", "jv", "jw", "ka", "kg", "ki", "kj", "kk", "kl", "km", "kn", "ko", "kr", "ks", "ku", "kv", "kw", "ky", "la", "lb", "lg", "li", "ln", "lo", "lt", "lu", "lv", "mg", "mh", "mi", "mk", "ml", "mn", "mo", "mr", "ms", "mt", "my", "na", "nb", "nd", "ne", "ng", "nl", "nn", "no", "nr", "nv", "ny", "oc", "oj", "om", "or", "os", "pa", "pi", "pl", "ps", "pt", "qu", "rm", "rn", "ro", "ru", "rw", "sa", "sc", "sd", "se", "sg", "sh", "si", "sk", "sl", "sm", "sn", "so", "sq", "sr", "ss", "st", "su", "sv", "sw", "ta", "te", "tg", "th", "ti", "tk", "tl", "tn", "to", "tr", "ts", "tt", "tw", "ty", "ug", "uk", "ur", "uz", "ve", "vi", "vo", "wa", "wo", "xh", "yi", "yo", "za", "zh", "zu"};
}

QString LexiconWorker::convertJsonToString(const QJsonObject &data) {
    QJsonDocument doc(data);
    return QString(doc.toJson(QJsonDocument::Indented));
}

QString LexiconWorker::convertJsonToString(const QJsonArray &data) {
    QJsonDocument doc(data);
    return QString(doc.toJson(QJsonDocument::Indented));
}

void LexiconWorker::emptyDatabase() {
    m_databaseComponent->emptyDatabase();
    getTopics();
}

void LexiconWorker::importFromLexiconDirectory(QString path) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
        m_databaseComponent = new DatabaseComponent;
    }

    // windows: remove path prefix "file:///"
    path = path.replace("file:///", "");

    qDebug() << "import from" << path;

    emit updatedProcessingFile("importing started...");

    QDirIterator iterator(path, QStringList() << "*.inc.php", QDir::NoFilter, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        QFileInfo fileInfo(file);

        QStringList pathParts = fileInfo.path().split("/");
        QString language = pathParts.count() > 0 ? pathParts.at(pathParts.count() - 1) : "";

        QStringList fileNameParts = fileInfo.fileName().split(".inc.php");
        QString topic = fileNameParts.count() > 0 ? fileNameParts.at(0) : "";

        int languageId = m_databaseComponent->getLanguageId(language);
        if (languageId < 0) {
            languageId = m_databaseComponent->addLanguage(language);
        }

        int topicId = m_databaseComponent->getTopicId(topic);
        if (topicId < 0) {
            topicId = m_databaseComponent->addTopic(topic);
        }

        emit updatedProcessingFile(fileInfo.fileName());

        // extract translation keys and translations
        processLexiconFile(languageId, topicId, file);
    }

    emit updatedProcessingFile("");

    emit importFromLexiconDirectoryFinished();

    emitUpdatedDatabaseStats();
}

void LexiconWorker::processLexiconFile(int &languageId, int &topicId, QFile &file) {
    if ((languageId <= 0) || (topicId <= 0)) {
        return;
    }

    file.open(QIODevice::ReadOnly);
    QFileInfo fileInfo(file);

    QByteArray fileBytes = file.readAll();

    // find package meta data
    QRegularExpression regularExpressionPackage("\\s*\\*\\s*@package\\s*(.*)\\s*\\*\\s*@author\\s*(.*)");

    QRegularExpressionMatchIterator iteratorPackage = regularExpressionPackage.globalMatch(fileBytes);

    while (iteratorPackage.hasNext()) {
        QRegularExpressionMatch match = iteratorPackage.next();

        QString name = match.captured(1).trimmed();
        QString author = match.captured(2).trimmed();

        if (name.isEmpty() || name.isNull()) {
            continue;
        }

        m_databaseComponent->updatePackage(name, "", author, 0);
    }

    // find translations
    QStringList regExps;
    regExps << "\\$_lang\\s*\\[\\s*'(\\w+)'\\s*\\]\\s*=\\s*'(.*?)';";
    regExps << "\\$_lang\\s*\\[\\s*\"(\\w+)\"\\s*\\]\\s*=\\s*\"(.*?)\";";
    regExps << "\\$_lang\\s*\\[\\s*\"(\\w+)\"\\s*\\]\\s*=\\s*'(.*?)';";
    regExps << "\\$_lang\\s*\\[\\s*'(\\w+)'\\s*\\]\\s*=\\s*\"(.*?)\";";

    for (int i = 0; i < regExps.count(); i++) {
        QRegularExpression regularExpressionTranslation(regExps.at(i), QRegularExpression::DotMatchesEverythingOption);

        QRegularExpressionMatchIterator iteratorTranslation = regularExpressionTranslation.globalMatch(fileBytes);

        while (iteratorTranslation.hasNext()) {
           QRegularExpressionMatch match = iteratorTranslation.next();

           QString translationKey = match.captured(1);
           QString translation = match.captured(2);

           if (translationKey.isEmpty() || translationKey.isNull() || translation.isEmpty() || translation.isNull()) {
               continue;
           }

           m_databaseComponent->addEntry(languageId, topicId, translationKey, translation);

           emit updatedProcessingFile(fileInfo.fileName() + " - " + translationKey);
        }
    }
}

void LexiconWorker::importFromSourceCodeDirectory(QString path) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
        m_databaseComponent = new DatabaseComponent;
    }

    // windows: remove path prefix "file:///"
    path = path.replace("file:///", "");

    qDebug() << "import from" << path;

    emit updatedProcessingFile("importing started...");

    QDirIterator iterator(path, QStringList() << "*.php" << "*.html", QDir::NoFilter, QDirIterator::Subdirectories);
    while (iterator.hasNext()) {
        QFile file(iterator.next());
        QFileInfo fileInfo(file);

        emit updatedProcessingFile(fileInfo.fileName());

        // extract translation keys
        processSourceCodeFile(file);
    }

    emit updatedProcessingFile("");

    emit importFromSourceCodeDirectoryFinished();

    emitUpdatedDatabaseStats();
}

void LexiconWorker::processSourceCodeFile(QFile &file) {
    file.open(QIODevice::ReadOnly);
    QFileInfo fileInfo(file);

    QByteArray fileBytes = file.readAll();

    // find translations
    QStringList regExps;
    // search in HTML file
    regExps << "\\Q[[\\E((?:(?:[^\\]\\]\\[\\[]++[\\s\\S]*?|(?R))*?))\\Q]]\\E";
    // search in PHP file
    regExps << "->lexicon\\('([^;)]+)'";

    for (int i = 0; i < regExps.count(); i++) {
        QRegularExpression regularExpressionTranslation(regExps.at(i), QRegularExpression::DotMatchesEverythingOption);

        QRegularExpressionMatchIterator iteratorTranslation = regularExpressionTranslation.globalMatch(fileBytes);

        QString translationNamespace = "";
        QString translationLanguage = "en";
        QString translationTopic = "default";

        while (iteratorTranslation.hasNext()) {
            QRegularExpressionMatch match = iteratorTranslation.next();

            QString found = match.captured(1);

            if (i == 0) { // search in HTML file
                QString token = found.sliced(0, 1);
                if (token == "%") {
                    QStringList parts = found.split("?");
                    if (parts.count() > 0) {
                        QString translationKey = parts[0].trimmed().sliced(1);
                        if (parts.count() == 2) {
                            QStringList parameters = parts[1].split("&");
                            for (int x = 0; x < parameters.count(); x++) {
                                QString parameter = parameters.at(x).trimmed();
                                QStringList parameterParts = parameter.split("=");
                                if (parameterParts.count() == 2) {
                                    QString parameterKey = parameterParts.at(0).trimmed();
                                    if (parameterKey == "topic") {
                                        translationTopic = parameterParts.at(1).trimmed().sliced(1).chopped(1);
                                    } else if (parameterKey == "language") {
                                        QString temp = parameterParts.at(1).trimmed().sliced(1).chopped(1);
                                        if (ianaLanguages.indexOf(temp) >= 0) {
                                            translationLanguage = temp;
                                        }
                                    } else if (parameterKey == "namespace") {
                                        translationNamespace = parameterParts.at(1).trimmed().sliced(1).chopped(1);
                                    }
                                }
                            }
                        }

                        // add translation
                        if (translationKey.length() > 0) {
                            if (!m_databaseComponent->isAlreadyUsedLanguageName(translationLanguage)) {
                                m_databaseComponent->addLanguage(translationLanguage);
                            }
                            if (!m_databaseComponent->isAlreadyUsedTopicName(translationTopic)) {
                                m_databaseComponent->addTopic(translationTopic);
                            }

                            int languageId = m_databaseComponent->getLanguageId(translationLanguage);
                            int topicId = m_databaseComponent->getTopicId(translationTopic);

                            m_databaseComponent->addEntry(languageId, topicId, translationKey, translationKey);

                            emit updatedProcessingFile(fileInfo.fileName() + " - " + translationKey);
                            //qDebug() << translationKey << translationNamespace << translationTopic << translationLanguage;
                        }
                    }
                }
            } else { // search in PHP file
                if (!m_databaseComponent->isAlreadyUsedLanguageName(translationLanguage)) {
                    m_databaseComponent->addLanguage(translationLanguage);
                }
                if (!m_databaseComponent->isAlreadyUsedTopicName(translationTopic)) {
                    m_databaseComponent->addTopic(translationTopic);
                }

                int languageId = m_databaseComponent->getLanguageId(translationLanguage);
                int topicId = m_databaseComponent->getTopicId(translationTopic);
                QString translationKey = found;

                m_databaseComponent->addEntry(languageId, topicId, translationKey, translationKey);

                emit updatedProcessingFile(fileInfo.fileName() + " - " + translationKey);
                //qDebug() << found;
            }
        }
    }
}

void LexiconWorker::exportAsFileStructure(QString path) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
       m_databaseComponent = new DatabaseComponent;
    }

    // windows: remove path prefix "file:///"
    path = path.replace("file:///", "");

    qDebug() << "export to" << path;

    // create "lexicon" sub-directory
    QString pathLexicon = path;
    /*
    if (!path.endsWith("/lexicon")) {
        pathLexicon += "/lexicon";
    }
    */
    QDir dirLexicon(pathLexicon);
    if (!dirLexicon.exists()) {
       if (!dirLexicon.mkdir(pathLexicon)) {
           qDebug() << "cannot create directory" << pathLexicon;
           emit exportAsFileStructureFinished();
           return;
       }
    }

    QJsonObject package = m_databaseComponent->getPackage();

    QVariantList languages = m_databaseComponent->getAllRows(m_databaseComponent->getTableLanguage(), QStringList("*"), "id", "ASC");
    QVariantList topics = m_databaseComponent->getAllRows(m_databaseComponent->getTableTopic(), QStringList("*"), "id", "ASC");

    for (int i = 0; i < languages.count(); i++) {
       QVariant variantLanguage = languages.at(i);
       QJsonObject jsonObjectLanguage = variantLanguage.toJsonObject();

       // create language sub-directory
       QString pathLanguage = pathLexicon + "/" + jsonObjectLanguage.value("name").toString();
       QDir dirLanguage(pathLanguage);
       if (!dirLanguage.exists()) {
           if (!dirLanguage.mkdir(pathLanguage)) {
               qDebug() << "cannot create directory" << pathLanguage;
               emit exportAsFileStructureFinished();
               return;
           }
       }

       QDateTime currentDate = QDateTime::currentDateTime();
       QString formattedDate = currentDate.toString("MMMM d, yyyy");

       for (int j = 0; j < topics.count(); j++) {
           QVariant variantTopic = topics.at(j);
           QJsonObject jsonObjectTopic = variantTopic.toJsonObject();

           // create topic file
           QString pathTopic = pathLanguage + "/" + jsonObjectTopic.value("name").toString() + ".inc.php";
           QFile fileTopic(pathTopic);
           if (!fileTopic.open(QIODevice::WriteOnly | QIODevice::Text)) {
               qDebug() << "cannot create file" << pathTopic;
               emit exportAsFileStructureFinished();
               return;
           }

           QTextStream out(&fileTopic);
           out << "<?php\n/**\n" << " * " << jsonObjectLanguage.value("name").toString() << " " << jsonObjectTopic.value("name").toString() << "\n *\n * @package " << package.value("name").toString() << "\n * @author " << package.value("author").toString() << "\n *\n * @description " << package.value("description").toString() << "\n *\n * @date " << formattedDate << "\n *\n */\n\n";

           QVariantList translations = m_databaseComponent->getTopicEntries(jsonObjectLanguage.value("id").toInt(), jsonObjectTopic.value("id").toInt());
           for (int k = 0; k < translations.count(); k++) {
               QVariant variantTranslation = translations.at(k);
               QJsonObject jsonObjectTranslation = variantTranslation.toJsonObject();

               out << "$_lang[" << m_stringLiteralChar << jsonObjectTranslation.value("translation_key").toString() << m_stringLiteralChar << "] = " << m_stringLiteralChar << jsonObjectTranslation.value("translation").toString() << m_stringLiteralChar << ";\n";
           }

           fileTopic.close();
       }
    }

    emit exportAsFileStructureFinished();
}

void LexiconWorker::saveTopicToFileStructure(int topicId, QString pathLexicon) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
       m_databaseComponent = new DatabaseComponent;
    }

    QJsonObject jsonObjectTopic = m_databaseComponent->getRowById(m_databaseComponent->getTableTopic(), topicId);

    // windows: remove path prefix "file:///"
    pathLexicon = pathLexicon.replace("file:///", "");

    qDebug() << "save topic" << jsonObjectTopic.value("name") << "to" << pathLexicon;

    // create lexicon path
    QDir dirLexicon(pathLexicon);
    if (!dirLexicon.exists()) {
       if (!dirLexicon.mkdir(pathLexicon)) {
           qDebug() << "cannot create directory" << pathLexicon;
           emit saveTopicToFileStructureFinished();
           return;
       }
    }

    QJsonObject package = m_databaseComponent->getPackage();

    QVariantList languages = m_databaseComponent->getAllRows(m_databaseComponent->getTableLanguage(), QStringList("*"), "id", "ASC");

    for (int i = 0; i < languages.count(); i++) {
       QVariant variantLanguage = languages.at(i);
       QJsonObject jsonObjectLanguage = variantLanguage.toJsonObject();

       // create language sub-directory
       QString pathLanguage = pathLexicon + "/" + jsonObjectLanguage.value("name").toString();
       QDir dirLanguage(pathLanguage);
       if (!dirLanguage.exists()) {
           if (!dirLanguage.mkdir(pathLanguage)) {
               qDebug() << "cannot create directory" << pathLanguage;
               emit saveTopicToFileStructureFinished();
               return;
           }
       }

       QDateTime currentDate = QDateTime::currentDateTime();
       QString formattedDate = currentDate.toString("MMMM d, yyyy");

       // create topic file
       QString pathTopic = pathLanguage + "/" + jsonObjectTopic.value("name").toString() + ".inc.php";
       QFile fileTopic(pathTopic);
       if (!fileTopic.open(QIODevice::WriteOnly | QIODevice::Text)) {
           qDebug() << "cannot create file" << pathTopic;
           emit saveTopicToFileStructureFinished();
           return;
       }

       QTextStream out(&fileTopic);
       out << "<?php\n/**\n" << " * " << jsonObjectLanguage.value("name").toString() << " " << jsonObjectTopic.value("name").toString() << "\n *\n * @package " << package.value("name").toString() << "\n * @author " << package.value("author").toString() << "\n *\n * @description " << package.value("description").toString() << "\n *\n * @date " << formattedDate << "\n *\n */\n\n";

       QVariantList translations = m_databaseComponent->getTopicEntries(jsonObjectLanguage.value("id").toInt(), jsonObjectTopic.value("id").toInt());
       for (int k = 0; k < translations.count(); k++) {
           QVariant variantTranslation = translations.at(k);
           QJsonObject jsonObjectTranslation = variantTranslation.toJsonObject();

           out << "$_lang[" << m_stringLiteralChar << jsonObjectTranslation.value("translation_key").toString() << m_stringLiteralChar << "] = " << m_stringLiteralChar << jsonObjectTranslation.value("translation").toString() << m_stringLiteralChar << ";\n";
       }

       fileTopic.close();
    }

    emit saveTopicToFileStructureFinished();
}

void LexiconWorker::emitUpdatedDatabaseStats() {
    QJsonObject data;

    // counts
    data.insert("languageCount", QJsonValue(m_databaseComponent->getLanguageCount()));
    data.insert("topicCount", QJsonValue(m_databaseComponent->getTopicCount()));
    data.insert("entryCount", QJsonValue(m_databaseComponent->getEntryCount()));
    data.insert("missingTranslationCount", QJsonValue(m_databaseComponent->getMissingTranslationCount()));

    // languages
    QVariantList languages = m_databaseComponent->getAllRows(m_databaseComponent->getTableLanguage(), QStringList("*"), "name", "ASC");
    QJsonArray jsonArray;
    for (int i = 0; i < languages.count(); i++) {
       QJsonObject language = languages.at(i).toJsonObject();

       jsonArray.append(language);
    }
    data.insert("languages", jsonArray);

    // package
    QJsonObject package = m_databaseComponent->getPackage();
    data.insert("packageName", QJsonValue(package.value("name").toString()));
    data.insert("author", QJsonValue(package.value("author").toString()));
    data.insert("description", QJsonValue(package.value("description").toString()));

    emit updatedDatabaseStats(convertJsonToString(data));
}

void LexiconWorker::addLanguage(QString language) {
    int languageId = m_databaseComponent->addLanguage(language);

    emit addLanguageResult(languageId);

    emitUpdatedDatabaseStats();
}

void LexiconWorker::addTopic(QString topic) {
    int topicId = m_databaseComponent->addTopic(topic);

    emitUpdatedDatabaseStats();
    emit addTopicResult(topicId);

    getTopics();
}

void LexiconWorker::addEntry(int languageId,
                             int topicId,
                             QString translationKey,
                             QString translation) {
    int entryId = m_databaseComponent->addEntry(languageId, topicId, translationKey, translation);

    emit addEntryResult(entryId);
    emitUpdatedDatabaseStats();
}

void LexiconWorker::updateEntry(int entryId,
                                QString translationKey,
                                QString translation) {
    m_databaseComponent->updateEntry(entryId, translationKey, translation);

    emit updateEntryResult(entryId);
    emitUpdatedDatabaseStats();
}

void LexiconWorker::updateTopic(int topicId,
                                QString name) {
    m_databaseComponent->updateTopic(topicId, name);

    emit updateTopicResult(topicId);
    emitUpdatedDatabaseStats();

    getTopics();
}

void LexiconWorker::moveEntry(int entryId,
                              int topicId) {
    m_databaseComponent->moveEntry(entryId, topicId);

    emit moveEntryResult(entryId);
    emitUpdatedDatabaseStats();
}

void LexiconWorker::deleteEntry(int entryId) {
    m_databaseComponent->deleteEntry(entryId);

    emit deleteEntryResult(entryId);
    emitUpdatedDatabaseStats();
}

void LexiconWorker::deleteTopic(int topicId) {
    m_databaseComponent->deleteTopic(topicId);

    emit deleteTopicResult(topicId);
    emitUpdatedDatabaseStats();

    getTopics();
}

void LexiconWorker::deleteLanguage(int languageId) {
    m_databaseComponent->deleteLanguage(languageId);

    emit deleteLanguageResult(languageId);
    emitUpdatedDatabaseStats();
}

void LexiconWorker::updatePackage(QString name,
                                  QString description,
                                  QString author,
                                  int date) {
    m_databaseComponent->updatePackage(name, description, author, date);

    QJsonObject data;
    data.insert("name", QJsonValue(name));
    data.insert("description", QJsonValue(description));
    data.insert("author", QJsonValue(author));
    data.insert("date", QJsonValue(date));

    emit updatePackageResult(convertJsonToString(data));
}

void LexiconWorker::updateSetting(QString key,
                                  QString value) {
    m_databaseComponent->updateSetting(key, value);

    QJsonObject data;
    data.insert("key", QJsonValue(key));
    data.insert("value", QJsonValue(value));

    emit updateSettingResult(convertJsonToString(data));
}

void LexiconWorker::getSetting(QString key) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
       m_databaseComponent = new DatabaseComponent;
    }

    QJsonObject setting = m_databaseComponent->getSetting(key);

    emit getSettingResult(convertJsonToString(setting));
}

void LexiconWorker::getTopics() {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
       m_databaseComponent = new DatabaseComponent;
    }

    QVariantList topics = m_databaseComponent->getAllRows("topic", QStringList() << "*", "name", "ASC");

    QJsonArray jsonArray;
    for (int i = 0; i < topics.count(); i++) {
        QVariant v = topics.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit getTopicsResult(convertJsonToString(jsonArray));
    emitUpdatedDatabaseStats();
}

void LexiconWorker::getTopicEntries(int topicId) {
    QVariantList entries = m_databaseComponent->getTopicEntries(topicId);

    QJsonArray jsonArray;
    for (int i = 0; i < entries.count(); i++) {
        QVariant v = entries.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit getTopicEntriesResult(convertJsonToString(jsonArray));
}

void LexiconWorker::getLanguages() {
    QVariantList languages = m_databaseComponent->getLanguages();

    QJsonArray jsonArray;
    for (int i = 0; i < languages.count(); i++) {
        QVariant v = languages.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit getLanguagesResult(convertJsonToString(jsonArray));
}

void LexiconWorker::searchEntries(QString searchTerm, QString orderBy, QString sortOrder) {
    QVariantList entries = m_databaseComponent->searchEntries(searchTerm, orderBy, sortOrder);

    QJsonArray jsonArray;
    for (int i = 0; i < entries.count(); i++) {
        QVariant v = entries.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit searchEntriesResult(convertJsonToString(jsonArray));
}

void LexiconWorker::getMissingTranslations() {
    QVariantList missingTranslations = m_databaseComponent->getMissingTranslations();

    QJsonArray jsonArray;
    for (int i = 0; i < missingTranslations.count(); i++) {
        QVariant v = missingTranslations.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit getMissingTranslationsResult(convertJsonToString(jsonArray));
}

void LexiconWorker::isAlreadyUsedTranslationKey(QString translationKey) {
    QVariantList topicNames = m_databaseComponent->isAlreadyUsedTranslationKey(translationKey);

    QJsonArray jsonArray;
    for (int i = 0; i < topicNames.count(); i++) {
        QVariant v = topicNames.at(i);
        QJsonObject obj = v.toJsonObject();
        jsonArray.append(obj);
    }

    emit isAlreadyUsedTranslationKeyResult(convertJsonToString(jsonArray));
}

void LexiconWorker::isAlreadyUsedTopicName(QString topicName) {
    QJsonObject data;
    data.insert("isAlreadyUsedTopicName", QJsonValue(m_databaseComponent->isAlreadyUsedTopicName(topicName)));

    emit isAlreadyUsedTopicNameResult(convertJsonToString(data));
}

void LexiconWorker::isAlreadyUsedLanguageName(QString languageName) {
    // is called on separate thread - the database connection MUST be created on that separate thread
    if (m_databaseComponent == nullptr) {
        m_databaseComponent = new DatabaseComponent;
    }

    QJsonObject data;
    data.insert("isAlreadyUsedLanguageName", QJsonValue(m_databaseComponent->isAlreadyUsedLanguageName(languageName)));

    emit isAlreadyUsedLanguageNameResult(convertJsonToString(data));
}
