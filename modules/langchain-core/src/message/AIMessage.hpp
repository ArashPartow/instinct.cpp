//
// Created by RobinQu on 2024/1/15.
//

#ifndef AIMESSAGE_H
#define AIMESSAGE_H
#include "Message.hpp"


LC_CORE_NS {
    class AIMessage : public Message {
    public:
        bool example;

        explicit AIMessage(std::string content, const bool example = false)
            : Message(std::move(content), kAIMessageType),
              example(example) {
        }

        ~AIMessage() override = default;

        std::string ToString() override;
    };

    inline std::string AIMessage::ToString() {
        return fmt::format("{role}: {content}", fmt::arg("role", "Asistant"), fmt::arg("content", GetContent()));
    }
} // core
// langchain

#endif //AIMESSAGE_H
