#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QLocale>
#include <QTranslator>
#include <QQmlContext>
#include <QIcon>
#include "lexiconprocessor.h"

int main(int argc, char *argv[]) {
    // clean / sharp text
    qputenv("QML_DISABLE_DISTANCEFIELD", "1");

    QGuiApplication app(argc, argv);

    app.setWindowIcon(QIcon(":images/modx-logo-color.svg"));

    // translation
    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ModxLinguist_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    // register qml types
    qmlRegisterType<LexiconProcessor>("ModxLinguist.LexiconProcessor", 1, 0, "LexiconProcessor");

    // start application
    QQmlApplicationEngine engine;

    LexiconProcessor* lexiconProcessor = new LexiconProcessor();
    engine.rootContext()->setContextProperty("lexiconProcessor", lexiconProcessor);

    const QUrl url(u"qrc:/ModxLinguist/qml/main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
