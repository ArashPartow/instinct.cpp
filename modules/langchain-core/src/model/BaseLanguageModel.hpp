//
// Created by RobinQu on 2024/1/12.
//

#ifndef BASELANGUAGEMODEL_H
#define BASELANGUAGEMODEL_H
#include <ranges>

#include "ChatGeneration.hpp"
#include "LLMResult.hpp"
#include "LLMRuntimeOptions.hpp"
#include "prompt/PromptValue.hpp"
#include "chain/Chain.hpp"
#include "Forwards.hpp"
#include "message/AIMessage.hpp"
#include "message/ChatMessage.hpp"
#include "message/FunctionMessage.hpp"
#include "message/HumanMessage.hpp"
#include "message/Message.hpp"
#include "message/SystemMessage.hpp"
#include "prompt/ChatPromptValue.h"
#include "prompt/StringPromptValue.h"
#include "tools/StringUtils.hpp"
#include "Forwards.hpp"


LC_CORE_NS {

    static auto conv_language_model_input_to_prompt_value = overloaded{
        [](const StringPromptValue& v) { return PromptValueVairant{v}; },

        [](const ChatPromptValue& v) { return PromptValueVairant(v); },

        [](const std::string& v) { return PromptValueVairant{StringPromptValue(v)}; },
        [](const MessageVariants& v) {
            return PromptValueVairant{ChatPromptValue(v)};
        }
    };

    template<typename LanguageModelOutput>
    class BaseLanguageModel : public Chain<LanguageModelInput, LanguageModelOutput, LLMRuntimeOptions> {
    public:
        // BaseLanguageModel() = default;
        // BaseLanguageModel(const BaseLanguageModel&) = delete;
        // BaseLanguageModel(BaseLanguageModel&&) = delete;

        virtual LLMResult GeneratePrompts(
            const std::vector<PromptValueVairant>& prompts,
            const LLMRuntimeOptions& runtime_options
        ) = 0;

        virtual std::string Predict(
            const std::string& text,
            const LLMRuntimeOptions& runtime_options
        ) = 0;

        virtual MessageVariant PredictMessages(
            const std::vector<MessageVariant>& messages,
            const LLMRuntimeOptions& runtime_options
        ) = 0;

        virtual std::vector<TokenId> GetTokenIds(const std::string& text) = 0;

        virtual TokenSize GetTokenCount(const std::string& text) = 0;

        virtual TokenSize GetTokenCount(const std::vector<BaseMessagePtr>& messages) = 0;
    };
} // core
// langchian

#endif //BASELANGUAGEMODEL_H
