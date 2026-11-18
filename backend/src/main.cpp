#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include <QtWebEngineQuick/qtwebenginequickglobal.h>

int main(int argc, char *argv[]) {
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--ignore-gpu-blocklist "
                                          "--enable-gpu-rasterization "
                                          "--enable-zero-copy");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QtWebEngineQuick::initialize();

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("WordSprint"));
    app.setOrganizationName(QStringLiteral("WordSprint"));

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

    QQmlApplicationEngine engine;
    engine.setInitialProperties({{QStringLiteral("initialUrl"), initialUrl}});
    engine.rootContext()->setContextProperty(QStringLiteral("initialUrl"), initialUrl);

    const QUrl url(QStringLiteral("qrc:/qt/qml/WordSprint/qml/Main.qml"));
    QObject::connect(
            &engine, &QQmlApplicationEngine::objectCreated, &app,
            [url](QObject *obj, const QUrl &objUrl) {
                if (!obj && url == objUrl)
                    QCoreApplication::exit(-1);
            },
            Qt::QueuedConnection);
        // Initialize SQLite database and ViewModel
    // Determine database file path (in application directory)
    const QString dbPath = QCoreApplication::applicationDirPath() + "/wordsprint.db";
    auto dbMgr = std::make_shared<DatabaseManager>(dbPath);
    // Load schema if external .sql files exist, otherwise create tables programmatically
    const QString schemaFile = QCoreApplication::applicationDirPath() + "/schema.sql";
    const QString seedFile = QCoreApplication::applicationDirPath() + "/seed.sql";
    if (QFileInfo::exists(schemaFile)) {
        QFile f(schemaFile);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString sql = QTextStream(&f).readAll();
            QSqlQuery q(dbMgr->db());
            for (const QString &stmt : sql.split(';', Qt::SkipEmptyParts)) {
                if (!stmt.trimmed().isEmpty()) {
                    if (!q.exec(stmt)) {
                        qWarning() << "Failed to exec schema stmt:" << stmt << q.lastError();
                    }
                }
            }
            f.close();
        }
    } else {
        // Fallback: create minimal tables
        QSqlQuery q(dbMgr->db());
        q.exec("CREATE TABLE IF NOT EXISTS levels (id INTEGER PRIMARY KEY, code TEXT, name TEXT, description TEXT);");
        q.exec("CREATE TABLE IF NOT EXISTS topics (id INTEGER PRIMARY KEY, name TEXT, description TEXT);");
        q.exec("CREATE TABLE IF NOT EXISTS vocabularies (id INTEGER PRIMARY KEY, word TEXT, phonetic TEXT, part_of_speech TEXT, meaning_vi TEXT, definition_en TEXT, example_sentence TEXT, example_translation TEXT, level_id INTEGER, topic_id INTEGER, created_at INTEGER, FOREIGN KEY(level_id) REFERENCES levels(id), FOREIGN KEY(topic_id) REFERENCES topics(id));");
    }
    if (QFileInfo::exists(seedFile)) {
        QFile f(seedFile);
        if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
            const QString sql = QTextStream(&f).readAll();
            QSqlQuery q(dbMgr->db());
            for (const QString &stmt : sql.split(';', Qt::SkipEmptyParts)) {
                if (!stmt.trimmed().isEmpty()) {
                    if (!q.exec(stmt)) {
                        qWarning() << "Failed to exec seed stmt:" << stmt << q.lastError();
                    }
                }
            }
            f.close();
        }
    }
    // Create repository and view model
    auto vocabRepo = std::make_unique<VocabularyRepository>(dbMgr);
    auto *viewModel = new VocabularyViewModel(dbMgr, nullptr);
    // inject repository (viewModel owns repo internally)
    // expose to QML
    engine.rootContext()->setContextProperty(QStringLiteral("vocabModel"), viewModel);
    // Continue loading QML
    engine.load(url);
    return app.exec();

    return app.exec();
}
