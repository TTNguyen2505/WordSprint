// Topic.h
// Domain model for the 'topics' table.
// Pure C++ struct without Qt dependencies.

#pragma once

#include <string>

struct Topic {
    int id{};                // Primary key
    std::string name;        // Topic name
    std::string description;// Optional description
};
