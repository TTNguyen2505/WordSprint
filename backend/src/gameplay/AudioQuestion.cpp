#include "AudioQuestion.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QUrl>

#include <utility>

namespace ReactFrontend_DomainLayer {
    namespace {
        int replayLimit(CEFRLevel level) {
            switch (level) {
                case CEFRLevel::A1:
                case CEFRLevel::A2:
                    return 3;
                case CEFRLevel::B1:
                case CEFRLevel::B2:
                    return 2;
                case CEFRLevel::C1:
                case CEFRLevel::C2:
                    return 1;
            }
            return 0;
        }

        QString audioSource(const std::string &path) {
            const QString input = QString::fromStdString(path).trimmed();
            if (input.isEmpty() || input.contains(QChar(0))) {
                return {};
            }

            QStringList candidates;
            if (input.startsWith(QStringLiteral("qrc:/"))) {
                candidates.append(QStringLiteral(":") + QUrl(input).path());
            } else if (input.startsWith(QStringLiteral(":/"))) {
                candidates.append(input);
            } else if (input.startsWith(QStringLiteral("file:"))) {
                const QString localPath = QUrl(input).toLocalFile();
                if (!localPath.isEmpty()) {
                    candidates.append(localPath);
                }
            } else {
                candidates.append(input);
                if (!QDir::isAbsolutePath(input)) {
                    candidates.append(QDir::current().filePath(input));
                    const QString normalized = QDir::cleanPath(input);
                    candidates.append(QStringLiteral(":/") + normalized);
                    if (!normalized.startsWith(QStringLiteral("storage/"))) {
                        candidates.append(QStringLiteral(":/storage/") + normalized);
                    }
                }
            }

            for (const QString &candidate: candidates) {
                QFile file(candidate);
                if (!file.open(QIODevice::ReadOnly)) {
                    continue;
                }
                file.close();
                if (candidate.startsWith(QStringLiteral(":/"))) {
                    return QStringLiteral("qrc") + candidate;
                }
                return QUrl::fromLocalFile(QFileInfo(candidate).absoluteFilePath()).toString(QUrl::FullyEncoded);
            }
            return {};
        }
    } // namespace

    AudioQuestion::AudioQuestion(WordItem wordItem, CEFRLevel level) : PuzzleQuestion(std::move(wordItem)),
                                                                      maxReplays(replayLimit(level)),
                                                                      remainingReplayCount(maxReplays) {
        if (this->wordItem.audio_path.has_value()) {
            audioPath = audioSource(*this->wordItem.audio_path).toStdString();
        }
        autoPlayOnLoad = !audioPath.empty();

        // Audio questions must not retain or expose the image clue.
        hintImage.clear();
        this->wordItem.hint_image_path.clear();
    }

    std::string AudioQuestion::generatePrompt() { return {}; }

    bool AudioQuestion::checkAnswer(std::string input) { return matchesAnswer(std::move(input)); }

    bool AudioQuestion::playAudio() {
        if (!isValid()) {
            return false;
        }
        return consumeReplay();
    }

    bool AudioQuestion::consumeReplay() {
        if (!isValid() || remainingReplayCount <= 0) {
            return false;
        }
        --remainingReplayCount;
        return true;
    }

    const std::string &AudioQuestion::audioPathValue() const noexcept { return audioPath; }

    int AudioQuestion::maxReplaysCount() const noexcept { return maxReplays; }

    int AudioQuestion::remainingReplays() const noexcept { return remainingReplayCount; }

    bool AudioQuestion::autoPlay() const noexcept { return autoPlayOnLoad; }

    bool AudioQuestion::isValid() const noexcept { return !audioPath.empty(); }

} // namespace ReactFrontend_DomainLayer
