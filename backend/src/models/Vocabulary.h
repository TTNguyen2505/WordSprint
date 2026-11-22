#pragma once

#include <string>

struct Vocabulary {
    int id = 0;
    int topicId = 0;
    std::string word;
    std::string phonetic;
    std::string partOfSpeech;
    std::string cefrLevel;
    std::string meaning;
    std::string exampleSentence;
};
