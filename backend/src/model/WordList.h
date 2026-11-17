// WordList.h - In-memory list model for Word entities
#pragma once

#include <algorithm>
#include <string>
#include <vector>
#include "Word.h"

class WordList {
private:
    std::vector<Word> m_words;

public:
    WordList() = default;

    // Add a new word
    void add(const Word& word) { m_words.push_back(word); }

    // Remove word at index (if valid)
    bool removeAt(size_t index) {
        if (index >= m_words.size()) return false;
        m_words.erase(m_words.begin() + index);
        return true;
    }

    // Update word at index
    bool updateAt(size_t index, const Word& word) {
        if (index >= m_words.size()) return false;
        m_words[index] = word;
        return true;
    }

    // Find first word matching exact text, return index or npos
    size_t findByText(const std::string& text) const {
        for (size_t i = 0; i < m_words.size(); ++i) {
            if (m_words[i].text() == text) return i;
        }
        return static_cast<size_t>(-1);
    }

    // Sort by length (ascending)
    void sortByLength() {
        std::sort(m_words.begin(), m_words.end(), [](const Word& a, const Word& b) {
            return a.length() < b.length();
        });
    }

    // Reset with new data
    void reset(const std::vector<Word>& newData) {
        m_words = newData;
    }

    // Accessors
    const std::vector<Word>& data() const { return m_words; }
    size_t size() const { return m_words.size(); }
    const Word& at(size_t index) const { return m_words.at(index); }
};