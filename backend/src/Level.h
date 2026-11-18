// Level.h
// Domain model for the 'levels' table.
// Pure C++ struct without any Qt or UI dependencies.

#pragma once

#include <string>

struct Level {
    int id{};                // Primary key
    std::string code;        // Short code identifier
    std::string name;        // Human readable name
    std::string description;// Optional description
};
