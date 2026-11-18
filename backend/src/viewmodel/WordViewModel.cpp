// WordViewModel.cpp - MVVM bridge delegating data persistence to WordRepository
#include "WordViewModel.h"

WordViewModel::WordViewModel(QObject* parent)
    : WordViewModel(nullptr, parent) {}

WordViewModel::WordViewModel(WordRepository* repository, QObject* parent)
    : QObject(parent), m_repository(repository), m_score(0), m_won(false) {
    if (!m_repository) {
        m_repository = new WordRepository(this);
    }
    if (!m_repository->isInitialized()) {
        m_repository->initialize();
    }
    loadFromRepository();
}

void WordViewModel::loadFromRepository() {
    if (!m_repository || !m_repository->isInitialized()) {
        return;
    }
    m_list.reset(m_repository->getAllWords());
}

QStringList WordViewModel::words() const {
    QStringList list;
    list.reserve(static_cast<int>(m_list.size()));
    for (const auto& w : m_list.data()) {
        list << QString::fromStdString(w.text());
    }
    return list;
}

void WordViewModel::addWord(const QString& txt) {
    const QString trimmed = txt.trimmed();
    if (trimmed.isEmpty()) {
        return;
    }

    Word newWord(trimmed.toStdString());
    if (m_repository && m_repository->isInitialized()) {
        if (!m_repository->addWord(newWord)) {
            return;
        }
    }

    m_list.add(newWord);
    emit wordsChanged();
}

bool WordViewModel::removeWord(int index) {
    if (index < 0 || static_cast<size_t>(index) >= m_list.size()) {
        return false;
    }

    const int wordId = m_list.at(static_cast<size_t>(index)).id();
    if (m_repository && m_repository->isInitialized() && wordId > 0) {
        if (!m_repository->removeWord(wordId)) {
            return false;
        }
    }

    const bool ok = m_list.removeAt(static_cast<size_t>(index));
    if (ok) {
        emit wordsChanged();
    }
    return ok;
}

bool WordViewModel::updateWord(int index, const QString& txt) {
    const QString trimmed = txt.trimmed();
    if (trimmed.isEmpty() || index < 0 || static_cast<size_t>(index) >= m_list.size()) {
        return false;
    }

    Word updated = m_list.at(static_cast<size_t>(index));
    updated.setText(trimmed.toStdString());

    if (m_repository && m_repository->isInitialized() && updated.id() > 0) {
        if (!m_repository->updateWord(updated.id(), updated.text())) {
            return false;
        }
    }

    const bool ok = m_list.updateAt(static_cast<size_t>(index), updated);
    if (ok) {
        emit wordsChanged();
    }
    return ok;
}

void WordViewModel::sortByLength() {
    m_list.sortByLength();
    emit wordsChanged();
}

void WordViewModel::resetData(const QStringList& newWords) {
    std::vector<Word> vec;
    vec.reserve(static_cast<size_t>(newWords.size()));
    for (const auto& s : newWords) {
        const QString trimmed = s.trimmed();
        if (!trimmed.isEmpty()) {
            vec.emplace_back(trimmed.toStdString());
        }
    }

    if (m_repository && m_repository->isInitialized()) {
        m_repository->replaceAllWords(vec);
        loadFromRepository();
    } else {
        m_list.reset(vec);
    }
    emit wordsChanged();
}

void WordViewModel::filterByCefrLevel(const QString& cefrLevel) {
    if (!m_repository || !m_repository->isInitialized()) {
        return;
    }

    const QString level = cefrLevel.trimmed().toUpper();
    if (level.isEmpty() || level == QStringLiteral("ALL")) {
        loadFromRepository();
    } else {
        m_list.reset(m_repository->getWordsByCefrLevel(level.toStdString()));
    }
    emit wordsChanged();
}

void WordViewModel::reloadWords() {
    loadFromRepository();
    emit wordsChanged();
}

void WordViewModel::finishGame(int finalScore, bool isWon) {
    m_score = finalScore;
    m_won = isWon;
    emit scoreChanged();
    emit resultChanged();
}

void WordViewModel::resetGame() {
    m_score = 0;
    m_won = false;
    emit scoreChanged();
    emit resultChanged();

    if (m_repository && m_repository->isInitialized()) {
        if (m_repository->countWords() == 0) {
            m_repository->resetToSeedData();
        }
        loadFromRepository();
    }
    emit wordsChanged();
}
