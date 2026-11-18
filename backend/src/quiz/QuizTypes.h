#pragma once

#include <QObject>

class QuizTypes {
    Q_GADGET
public:
    enum class Type {
        Scramble = 1,
        FillInBlank = 2,
        Listening = 3,
        Mixed = 4
    };
    Q_ENUM(Type)
};

using QuizType = QuizTypes::Type;
