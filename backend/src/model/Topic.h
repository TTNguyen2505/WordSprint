// Topic.h - Pure C++ domain struct for a vocabulary topic entity
#pragma once
#include <string>
#include <utility>

struct Topic {
    int id = 0;
    std::string name;

    Topic() = default;
    Topic(int topicId, std::string topicName)
        : id(topicId), name(std::move(topicName)) {}
};
