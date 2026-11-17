#include <iostream>
#include "Word.h"
#include "WordList.h"

int main() {
    WordList list;
    list.add(Word("hello"));
    list.add(Word("world"));
    list.add(Word("antigravity"));
    std::cout << "Initial size: " << list.size() << "\n";
    for (const auto& w : list.data()) {
        std::cout << w.text() << " (" << w.length() << ")\n";
    }
    list.sortByLength();
    std::cout << "After sorting by length:\n";
    for (const auto& w : list.data()) {
        std::cout << w.text() << " (" << w.length() << ")\n";
    }
    return 0;
}
