// Word.h - Data Model for a word entity
#pragma once
#include <string>

class Word {
private:
    std::string m_text;
    int m_length;
public:
    // Constructor
    explicit Word(const std::string& text = "")
        : m_text(text), m_length(static_cast<int>(text.size())) {}

    // Getter for text
    const std::string& text() const { return m_text; }
    // Setter for text (updates length)
    void setText(const std::string& text) {
        m_text = text;
        m_length = static_cast<int>(text.size());
    }

    // Getter for length
    int length() const { return m_length; }
};
