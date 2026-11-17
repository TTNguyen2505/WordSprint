#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QUrl>
#include "viewmodel/AppViewModel.h"
#include "viewmodel/GameplayViewModel.h"
#include "model/MockVocabularyRepository.h"
#if defined(HAVE_WEBENGINEQUICK)
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#endif

int main(int argc, char *argv[]) {


    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--ignore-gpu-blocklist "
                                          "--enable-gpu-rasterization "
                                          "--enable-zero-copy");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    #if defined(HAVE_WEBENGINEQUICK)
    QtWebEngineQuick::initialize();
#endif

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
    // Register ViewModels with QML
    AppViewModel appVM;
    MockVocabularyRepository repo;
    GameplayViewModel gameplayVM(&repo);
    engine.rootContext()->setContextProperty("appVM", &appVM);
    engine.rootContext()->setContextProperty("gameplayVM", &gameplayVM);
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
    engine.load(url);

    return app.exec();
}
