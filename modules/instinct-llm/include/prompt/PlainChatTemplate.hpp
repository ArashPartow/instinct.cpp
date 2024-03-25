//
// Created by RobinQu on 2024/3/9.
//

#ifndef PLAINCHATTEMPLATE_HPP
#define PLAINCHATTEMPLATE_HPP

#include "BaseChatPromptTemplate.hpp"
#include "LLMGlobals.hpp"
#include "prompt/MessageUtils.hpp"
#include "functional/JSONContextPolicy.hpp"

namespace INSTINCT_LLM_NS {
    using namespace INSTINCT_CORE_NS;


    class PlainChatTemplate final: public BaseChatPromptTemplate {
        std::vector<MessageLikeVariant> messages_;

    public:
        explicit PlainChatTemplate(const std::vector<MessageLikeVariant> &messages,
                          const PromptTemplateOptions &options = {})
                : BaseChatPromptTemplate(options), messages_(messages) {}


        MessageList FormatMessages(const TemplateVariablesPtr& variables) override {
            MessageList message_list;
            for (const auto& message_like: messages_) {
                if (std::holds_alternative<Message>(message_like)) {
                    message_list.add_messages()->CopyFrom(std::get<Message>(message_like));
                }
                if (std::holds_alternative<ChatPromptTemplatePtr>(message_like)) {
                    auto chat_prompt_template = std::get<ChatPromptTemplatePtr>(message_like);
                    auto part_list = chat_prompt_template->FormatMessages(variables);
                    message_list.MergeFrom(part_list);
                }
                // if message is history_placeholder, copy history messages from context variables
            }

            // format content field of each message
            for (int i=0;i<message_list.messages_size();++i) {
                auto* msg = message_list.mutable_messages(i);
                msg->set_content(MessageUtils::FormatString(msg->content(), variables));
            }
            return message_list;
        }

    };


}


#endif //PLAINCHATTEMPLATE_HPP
