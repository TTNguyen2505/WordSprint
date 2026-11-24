#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QStandardPaths>
#include <QUrl>
#include <QWebChannel>
#include <QStringList>

#include <memory>

#if __has_include(<QtWebEngineQuick/qtwebenginequickglobal.h>)
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#define HAS_WEBENGINE 1
#endif

#include "BridgeController.h"
#include "database/DatabaseManager.h"
#include "repositories/SqliteVocabularyRepository.h"

int main(int argc, char *argv[]) {
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--ignore-gpu-blocklist "
                                          "--enable-gpu-rasterization "
                                          "--enable-zero-copy");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#if HAS_WEBENGINE
    QtWebEngineQuick::initialize();
#endif

    // Keep the database objects alive until after QGuiApplication is destroyed.
    std::unique_ptr<DatabaseManager> databaseManager;
    std::unique_ptr<SqliteVocabularyRepository> vocabularyRepository;

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("WordSprint"));
    app.setOrganizationName(QStringLiteral("WordSprint"));

    databaseManager = std::make_unique<DatabaseManager>();

    QString dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty()) {
        dataDirectory = QCoreApplication::applicationDirPath();
    }
    if (!QDir().mkpath(dataDirectory)) {
        qWarning() << "WordSprint: Could not create application data directory:" << dataDirectory;
    }

    const QString databasePath = QDir(dataDirectory).filePath(QStringLiteral("wordsprint.db"));
    if (!databaseManager->openDatabase(databasePath.toStdString())) {
        qWarning() << "WordSprint: Could not open the vocabulary database:"
                   << QString::fromStdString(databaseManager->lastError());
    } else if (databaseManager->connection().tables().isEmpty()) {
        if (!databaseManager->initializeDatabase(QStringLiteral(":/db/schema.sql"),
                                                 QStringLiteral(":/db/seed_data.sql"))) {
            qWarning() << "WordSprint: Could not initialize the vocabulary database:"
                       << QString::fromStdString(databaseManager->lastError());
        }
    } else {
        qInfo() << "WordSprint: Existing database tables found; skipping schema and seed initialization.";
    }

    vocabularyRepository = std::make_unique<SqliteVocabularyRepository>(*databaseManager);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("WordSprint Desktop Client"));
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption debugOption(QStringList() << QStringLiteral("d") << QStringLiteral("debug"),
                                   QStringLiteral("Enable debug mode to live preview from Vite dev server."));
    parser.addOption(debugOption);

    QCommandLineOption urlOption(QStringList() << QStringLiteral("u") << QStringLiteral("url"),
                                 QStringLiteral("Custom URL to load in WebEngineView."), QStringLiteral("url"));
    parser.addOption(urlOption);

    parser.process(app);

    bool isDebug = false;
#ifdef QT_DEBUG
    isDebug = true;
#endif
    if (parser.isSet(debugOption) || qEnvironmentVariableIsSet("VITE_DEV_SERVER_URL")) {
        isDebug = true;
    }

    QUrl initialUrl;
    if (parser.isSet(urlOption)) {
        initialUrl = QUrl::fromUserInput(parser.value(urlOption));
    } else if (isDebug) {
        const QString devUrl = qEnvironmentVariable("VITE_DEV_SERVER_URL", QStringLiteral("http://localhost:5173"));
        initialUrl = QUrl(devUrl);
    } else {
        if (QFileInfo::exists(QStringLiteral(":/dist/index.html"))) {
            initialUrl = QUrl(QStringLiteral("qrc:///dist/index.html"));
        } else {
            const QString distRelative =
                    QCoreApplication::applicationDirPath() + QStringLiteral("/../frontend/dist/index.html");
            const QFileInfo distFile(distRelative);
            if (distFile.exists()) {
                initialUrl = QUrl::fromLocalFile(distFile.canonicalFilePath());
            } else {
                const QFileInfo localDist(QStringLiteral("frontend/dist/index.html"));
                if (localDist.exists()) {
                    initialUrl = QUrl::fromLocalFile(localDist.canonicalFilePath());
                } else {
                    initialUrl = QUrl(QStringLiteral("http://localhost:5173"));
                }
            }
        }
    }

    // Set up BridgeController and QWebChannel
    BridgeController bridgeController(vocabularyRepository.get());
    QWebChannel webChannel;
    webChannel.registerObject(QStringLiteral("bridgeController"), &bridgeController);
    webChannel.registerObject(QStringLiteral("bridge"), &bridgeController);
    webChannel.registerObject(QStringLiteral("backend"), &bridgeController);

    // Initialise QQmlApplicationEngine with initialProperties (modern Qt, replacing setContextProperty)
    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {QStringLiteral("initialUrl"), initialUrl},
        {QStringLiteral("webChannel"), QVariant::fromValue(&webChannel)}
    });

    const QUrl url(QStringLiteral("qrc:/qt/qml/WordSprint/qml/Main.qml"));
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated, &app,
            [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
