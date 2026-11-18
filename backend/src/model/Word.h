// Word.h - Data Model for a word entity
#pragma once
#include <string>

class Word {
private:
    int m_id = 0;
    int m_topicId = 1;
    std::string m_text;
    int m_length = 0;
    std::string m_phonetic;
    std::string m_pos;
    std::string m_meaningEn;
    std::string m_meaningVi;
    std::string m_example;
    std::string m_cefrLevel = "A1";

public:
    explicit Word(const std::string& text = "")
        : m_text(text), m_length(static_cast<int>(text.size())) {}

    Word(int id, int topicId, const std::string& text, const std::string& phonetic,
         const std::string& pos, const std::string& meaningEn, const std::string& meaningVi,
         const std::string& example, const std::string& cefrLevel)
        : m_id(id),
          m_topicId(topicId),
          m_text(text),
          m_length(static_cast<int>(text.size())),
          m_phonetic(phonetic),
          m_pos(pos),
          m_meaningEn(meaningEn),
          m_meaningVi(meaningVi),
          m_example(example),
          m_cefrLevel(cefrLevel.empty() ? "A1" : cefrLevel) {}

    int id() const { return m_id; }
    void setId(int id) { m_id = id; }

    int topicId() const { return m_topicId; }
    void setTopicId(int topicId) { m_topicId = topicId; }

    const std::string& text() const { return m_text; }
    void setText(const std::string& text) {
        m_text = text;
        m_length = static_cast<int>(text.size());
    }

    int length() const { return m_length; }

    const std::string& phonetic() const { return m_phonetic; }
    void setPhonetic(const std::string& phonetic) { m_phonetic = phonetic; }

    const std::string& pos() const { return m_pos; }
    void setPos(const std::string& pos) { m_pos = pos; }

    const std::string& meaningEn() const { return m_meaningEn; }
    void setMeaningEn(const std::string& meaningEn) { m_meaningEn = meaningEn; }

    const std::string& meaningVi() const { return m_meaningVi; }
    void setMeaningVi(const std::string& meaningVi) { m_meaningVi = meaningVi; }

    const std::string& example() const { return m_example; }
    void setExample(const std::string& example) { m_example = example; }

    const std::string& cefrLevel() const { return m_cefrLevel; }
    void setCefrLevel(const std::string& cefrLevel) { m_cefrLevel = cefrLevel; }
};
