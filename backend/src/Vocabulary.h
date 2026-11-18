// Vocabulary.h
// Domain model for the 'vocabularies' table.
// Pure C++ struct without Qt or UI dependencies.

#pragma once

#include <string>
#include <chrono>

struct Vocabulary {
    int id{};                       // Primary key
    std::string word;               // Vocabulary word
    std::string phonetic;           // Phonetic transcription
    std::string part_of_speech;    // e.g., noun, verb
    std::string meaning_vi;         // Vietnamese meaning
    std::string definition_en;      // English definition
    std::string example_sentence;  // Example usage in English
    std::string example_translation;// Vietnamese translation of example
    int level_id{};                 // FK to levels.id
    int topic_id{};                 // FK to topics.id
    std::chrono::system_clock::time_point created_at; // Timestamp
};
