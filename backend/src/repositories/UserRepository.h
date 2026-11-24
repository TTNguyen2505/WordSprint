#pragma once

#include "../models/User.h"

#include <optional>
#include <string>
#include <vector>

class DatabaseManager;

class UserRepository final {
public:
    explicit UserRepository(DatabaseManager &databaseManager);

    std::optional<std::vector<User>> getAllUsers();
    std::optional<User> createUser(const std::string &username);
    bool updateAudioSettings(int userId, bool bgmOn, bool sfxOn, int bgmVol, int sfxVol);

private:
    DatabaseManager &m_databaseManager;
};
