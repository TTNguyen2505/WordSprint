#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

#include <memory>

#if __has_include(<QtWebEngineQuick/qtwebenginequickglobal.h>)
#include <QtWebEngineQuick/qtwebenginequickglobal.h>
#define HAS_WEBENGINE 1
#endif

#include "controller/BridgeController.h"
#include "core/ApplicationSetup.h"
#include "core/DatabaseManager.h"
#include "repositories/RecordRepository.h"
#include "repositories/StageRepository.h"
#include "repositories/UserRepository.h"

int main(int argc, char *argv[]) {
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--ignore-gpu-blocklist "
                                          "--enable-gpu-rasterization "
                                          "--enable-zero-copy");

    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

#if HAS_WEBENGINE
    QtWebEngineQuick::initialize();
#endif

    std::unique_ptr<DatabaseManager> databaseManager;
    std::unique_ptr<StageRepository> stageRepository;
    std::unique_ptr<RecordRepository> recordRepository;
    std::unique_ptr<UserRepository> userRepository;

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("WordSprint"));
    app.setOrganizationName(QStringLiteral("WordSprint"));

    databaseManager = std::make_unique<DatabaseManager>();
    ApplicationSetup::initializeDatabase(*databaseManager);

    stageRepository = std::make_unique<StageRepository>(*databaseManager);
    recordRepository = std::make_unique<RecordRepository>(*databaseManager);
    userRepository = std::make_unique<UserRepository>(*databaseManager);

    const QUrl initialUrl = ApplicationSetup::frontendUrl(app);

    BridgeController bridgeController(stageRepository.get(), recordRepository.get(), userRepository.get());

    QQmlApplicationEngine engine;
    engine.setInitialProperties({{QStringLiteral("initialUrl"), initialUrl},
                                 {QStringLiteral("bridgeController"), QVariant::fromValue(&bridgeController)}});

    const QUrl url(QStringLiteral("qrc:/qt/qml/WordSprint/src/Main.qml"));
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
