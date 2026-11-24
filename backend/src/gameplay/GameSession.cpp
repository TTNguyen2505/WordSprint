#include "GameSession.h"

#include "AudioQuestion.h"
#include "FillBlankQuestion.h"
#include "ScrambleQuestion.h"

#include "../models/CEFRLevel.h"

#include <QFile>
#include <QString>

#include <algorithm>
#include <array>
#include <cctype>
#include <functional>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace ReactFrontend_DomainLayer {
    namespace {
        enum class QuizMode { Scramble, FillBlank, Audio };

        std::mt19937 makeRandomEngine() {
            std::random_device randomDevice;
            std::seed_seq seed{randomDevice(), randomDevice(), randomDevice(), randomDevice()};
            return std::mt19937(seed);
        }

        bool resourceAvailable(const std::string &path) {
            if (path.empty()) {
                return false;
            }

            const QString resource = QString::fromStdString(path);
            if (resource.startsWith(QStringLiteral(":/"))) {
                return QFile::exists(resource);
            }
            if (resource.startsWith(QStringLiteral("qrc:/"))) {
                return QFile::exists(QStringLiteral(":") + resource.mid(4));
            }

            const QString qrcPath = resource.startsWith(QLatin1Char('/'))
                                            ? QStringLiteral(":") + resource
                                            : QStringLiteral(":/") + resource;
            return QFile::exists(qrcPath) || QFile::exists(resource);
        }

        bool hasAudio(const ::WordItem &word) {
            return word.audio_path.has_value() && resourceAvailable(*word.audio_path);
        }

        bool isLetter(char character) {
            return std::isalpha(static_cast<unsigned char>(character)) != 0;
        }

        bool canScramble(const ::WordItem &word) {
            if (!resourceAvailable(word.hint_image_path)) {
                return false;
            }
            std::vector<char> lettersInWord;
            for (const char character: word.target_word) {
                if (character == ' ') {
                    std::sort(lettersInWord.begin(), lettersInWord.end());
                    if (std::adjacent_find(lettersInWord.begin(), lettersInWord.end(), std::not_equal_to<>()) !=
                        lettersInWord.end()) {
                        return true;
                    }
                    lettersInWord.clear();
                } else if (isLetter(character)) {
                    lettersInWord.push_back(character);
                }
            }
            std::sort(lettersInWord.begin(), lettersInWord.end());
            return std::adjacent_find(lettersInWord.begin(), lettersInWord.end(), std::not_equal_to<>()) !=
                   lettersInWord.end();
        }

        bool canFillBlank(const ::WordItem &word) {
            return resourceAvailable(word.hint_image_path) &&
                   std::count_if(word.target_word.begin(), word.target_word.end(), isLetter) >= 2;
        }
    } // namespace

    void GameSession::startSession(::Stage stage, std::vector<::WordItem> words) {
        const std::optional<CEFRLevel> level = cefrLevelFromName(stage.cefr_level);
        if (!level.has_value()) {
            throw std::invalid_argument("GameSession requires a supported CEFR level.");
        }
        if (stage.question_count <= 0 || words.size() != static_cast<std::size_t>(stage.question_count)) {
            throw std::invalid_argument("GameSession requires exactly the configured number of words.");
        }

        this->stage = std::move(stage);
        questions.clear();
        currentIndex = 0;
        score = 0;
        currentStreak = 0;
        timeRemaining = this->stage.time_limit;
        isPaused = false;

        constexpr std::array<QuizMode, 3> modes = {QuizMode::Scramble, QuizMode::FillBlank, QuizMode::Audio};
        std::mt19937 randomEngine = makeRandomEngine();
        std::vector<QuizMode> bag;
        bag.reserve(words.size());
        for (std::size_t index = 0; index < words.size() / modes.size(); ++index) {
            bag.insert(bag.end(), modes.begin(), modes.end());
        }
        std::array<QuizMode, 3> remainderModes = modes;
        std::shuffle(remainderModes.begin(), remainderModes.end(), randomEngine);
        bag.insert(bag.end(), remainderModes.begin(), remainderModes.begin() + (words.size() % modes.size()));
        std::shuffle(bag.begin(), bag.end(), randomEngine);

        questions.reserve(words.size());
        for (std::size_t index = 0; index < words.size(); ++index) {
            ::WordItem &word = words[index];
            QuizMode mode = bag[index];

            std::vector<QuizMode> validModes;
            if (canScramble(word)) {
                validModes.push_back(QuizMode::Scramble);
            }
            if (canFillBlank(word)) {
                validModes.push_back(QuizMode::FillBlank);
            }
            if (hasAudio(word)) {
                validModes.push_back(QuizMode::Audio);
            }
            if (validModes.empty()) {
                throw std::invalid_argument("No quiz mode can generate a valid prompt for a word.");
            }
            if (std::find(validModes.begin(), validModes.end(), mode) == validModes.end()) {
                const bool missingAudioFallback = mode == QuizMode::Audio && !hasAudio(word);
                std::vector<QuizMode> fallbackModes;
                for (const QuizMode candidateMode: validModes) {
                    if (!missingAudioFallback || candidateMode != QuizMode::Audio) {
                        fallbackModes.push_back(candidateMode);
                    }
                }
                if (fallbackModes.empty()) {
                    throw std::invalid_argument("Audio fallback requires Scramble or FillBlank for this word.");
                }
                std::uniform_int_distribution<std::size_t> fallbackDistribution(0, fallbackModes.size() - 1);
                mode = fallbackModes[fallbackDistribution(randomEngine)];
            }

            switch (mode) {
                case QuizMode::Scramble:
                    questions.push_back(std::make_unique<ScrambleQuestion>(std::move(word)));
                    break;
                case QuizMode::FillBlank:
                    questions.push_back(std::make_unique<FillBlankQuestion>(std::move(word)));
                    break;
                case QuizMode::Audio:
                    questions.push_back(std::make_unique<AudioQuestion>(std::move(word)));
                    break;
            }
        }

        if (!questions.empty()) {
            questions.front()->status = QuestionStatus::CURRENT;
        }
    }

    const std::vector<std::unique_ptr<PuzzleQuestion>> &GameSession::getQuestions() const noexcept {
        return questions;
    }

} // namespace ReactFrontend_DomainLayer
