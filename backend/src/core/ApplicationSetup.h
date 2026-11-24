#pragma once

#include <QUrl>

class DatabaseManager;
class QCoreApplication;

namespace ApplicationSetup {

    void initializeDatabase(DatabaseManager &databaseManager);
    QUrl frontendUrl(const QCoreApplication &app);

} // namespace ApplicationSetup
