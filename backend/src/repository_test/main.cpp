#include <QCoreApplication>
#include <QFile>
#include <iostream>
#include "../repository/WordRepository.h"

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    const QString testDb = QCoreApplication::applicationDirPath() + QStringLiteral("/repo_test.db");
    QFile::remove(testDb);

    WordRepository repo;
    if (!repo.initialize(testDb)) {
        std::cerr << "FAIL: Could not initialize WordRepository\n";
        return 1;
    }

    // 1. Verify SQLite -> pure C++ Topic mapping
    const std::vector<Topic> topics = repo.getAllTopics();
    if (topics.size() < 6) {
        std::cerr << "FAIL: Expected at least 6 topics, got " << topics.size() << "\n";
        return 1;
    }
    std::cout << "Mapped Topics (" << topics.size() << "): first = [" << topics[0].id << "] "
              << topics[0].name << "\n";

    // 2. Verify SQLite -> pure C++ Word mapping
    const std::vector<Word> allWords = repo.getAllWords();
    if (allWords.size() != 60) {
        std::cerr << "FAIL: Expected 60 words from seed, got " << allWords.size() << "\n";
        return 1;
    }
    std::cout << "Mapped Words (" << allWords.size() << "): first = [" << allWords[0].id() << "] "
              << allWords[0].text() << " (" << allWords[0].cefrLevel() << " - "
              << allWords[0].meaningVi() << ")\n";

    // 3. Verify filtered query + CRUD persistence
    const std::vector<Word> b1Words = repo.getWordsByCefrLevel("B1");
    if (b1Words.size() != 10) {
        std::cerr << "FAIL: Expected 10 B1 words, got " << b1Words.size() << "\n";
        return 1;
    }

    Word customWord(0, topics[0].id, "resilience", "/rɪˈzɪliəns/", "noun", "ability to recover",
                    "khả năng phục hồi", "She showed great resilience.", "B2");
    if (!repo.addWord(customWord) || customWord.id() <= 0) {
        std::cerr << "FAIL: addWord failed\n";
        return 1;
    }
    if (!repo.updateWord(customWord.id(), "resilient") || !repo.removeWord(customWord.id())) {
        std::cerr << "FAIL: updateWord or removeWord failed\n";
        return 1;
    }

    QFile::remove(testDb);
    std::cout << "PASS: Repository / DAO mapping and CRUD checks passed!\n";
    return 0;
}
