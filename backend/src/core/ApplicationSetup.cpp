#include "ApplicationSetup.h"

#include "DatabaseManager.h"

#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QStringList>

void ApplicationSetup::initializeDatabase(DatabaseManager &databaseManager) {
    QString dataDirectory = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dataDirectory.isEmpty()) {
        dataDirectory = QCoreApplication::applicationDirPath();
    }
    if (!QDir().mkpath(dataDirectory)) {
        qWarning() << "WordSprint: Could not create application data directory:" << dataDirectory;
    }

    const QString databasePath = QDir(dataDirectory).filePath(QStringLiteral("wordsprint.db"));
    if (!databaseManager.openDatabase(databasePath.toStdString())) {
        qWarning() << "WordSprint: Could not open the vocabulary database:"
                   << QString::fromStdString(databaseManager.lastError());
        return;
    }

    const QString applicationDirectory = QCoreApplication::applicationDirPath();
    const QStringList databaseDirectories = {QDir(applicationDirectory).filePath(QStringLiteral("database")),
                                             QDir(applicationDirectory).filePath(QStringLiteral("../sql")),
                                             QDir(applicationDirectory).filePath(QStringLiteral("../../backend/sql")),
                                             QDir::current().filePath(QStringLiteral("backend/sql")),
                                             QDir::current().filePath(QStringLiteral("sql"))};

    QString schemaPath;
    QString appDataSeedPath;
    QString mockUsersSeedPath;
    for (const QString &directory: databaseDirectories) {
        const QFileInfo schemaInfo(QDir(directory).filePath(QStringLiteral("schema.sql")));
        const QFileInfo appDataSeedInfo(QDir(directory).filePath(QStringLiteral("seed_app_data.sql")));
        const QFileInfo mockUsersSeedInfo(QDir(directory).filePath(QStringLiteral("seed_mock_users.sql")));
        if (schemaInfo.isFile() && appDataSeedInfo.isFile() && mockUsersSeedInfo.isFile()) {
            schemaPath = schemaInfo.absoluteFilePath();
            appDataSeedPath = appDataSeedInfo.absoluteFilePath();
            mockUsersSeedPath = mockUsersSeedInfo.absoluteFilePath();
            break;
        }
    }

    if (!databaseManager.initializeDatabase(schemaPath.toStdString(), appDataSeedPath.toStdString(),
                                            mockUsersSeedPath.toStdString())) {
        qWarning() << "WordSprint: Could not initialize the vocabulary database:"
                   << QString::fromStdString(databaseManager.lastError());
        databaseManager.closeDatabase();
    }
}

QUrl ApplicationSetup::frontendUrl(const QCoreApplication &app) {
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

    if (parser.isSet(urlOption)) {
        return QUrl::fromUserInput(parser.value(urlOption));
    }
    if (isDebug) {
        const QString devUrl = qEnvironmentVariable("VITE_DEV_SERVER_URL", QStringLiteral("http://localhost:5173"));
        return QUrl(devUrl);
    }
    if (QFileInfo::exists(QStringLiteral(":/dist/index.html"))) {
        return QUrl(QStringLiteral("qrc:///dist/index.html"));
    }

    const QString distRelative =
            QCoreApplication::applicationDirPath() + QStringLiteral("/../frontend/dist/index.html");
    const QFileInfo distFile(distRelative);
    if (distFile.exists()) {
        return QUrl::fromLocalFile(distFile.canonicalFilePath());
    }
    const QFileInfo localDist(QStringLiteral("frontend/dist/index.html"));
    if (localDist.exists()) {
        return QUrl::fromLocalFile(localDist.canonicalFilePath());
    }
    return QUrl(QStringLiteral("http://localhost:5173"));
}
