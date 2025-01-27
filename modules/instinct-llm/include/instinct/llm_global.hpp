//
// Created by RobinQu on 2024/2/13.
//

#ifndef MODELGLOBALS_H
#define MODELGLOBALS_H


#include <instinct/llm.pb.h>
#include <instinct/agent.pb.h>
#include <instinct/core_global.hpp>
#include <instinct/tools/chrono_utils.hpp>
#include <instinct/tools/protobuf_utils.hpp>
#include <instinct/tools/snowflake_id_generator.hpp>
#include <instinct/tools/string_utils.hpp>

#define INSTINCT_LLM_NS instinct::llm

namespace INSTINCT_LLM_NS {
    using namespace INSTINCT_CORE_NS;

    using TokenSize = unsigned long;
    using TokenId = unsigned long;

    using Embedding = std::vector<float>;

    struct ChainOptions {

    };

    /**
     * LLM input variant
     */
    using PromptValueVariant = std::variant<
        StringPromptValue,
        ChatPromptValue,
        PromptValue,
        MessageList,
        Message,
        std::string
    >;


    /**
     * this should refer to a Generation
     */
    static const std::string DEFAULT_ANSWER_OUTPUT_KEY = "answer";


    /**
     * this should refer to a PromptValue
     */
    static const std::string DEFAULT_PROMPT_VARIABLE_KEY = "prompt";


    /**
     * this should refer to a string describing a question for retriever
     */
    static const std::string DEFAULT_QUESTION_INPUT_OUTPUT_KEY = "question";

    /**
     * this should refer to a string containing contextual information returned by retriever
     */
    static const std::string DEFAULT_CONTEXT_OUTPUT_KEY = "context";


    static const std::string DEFAULT_STANDALONE_QUESTION_INPUT_KEY = "standalone_question";


    using TemplateVariables = nlohmann::json;
    using TemplateVariablesPtr = std::shared_ptr<TemplateVariables>;

    using PromptExample = nlohmann::json;
    using PromptExamples = std::vector<PromptExample>;

    static TemplateVariablesPtr CreateTemplateVariable() {
        return std::make_shared<TemplateVariables>(nlohmann::json::parse("{}"));
    }



    inline std::ostream& operator<<(std::ostream& ostrm, const Message& msg) {
        ostrm << "Message[role=" << msg.role() << ", content=" << msg.content() << "]";
        return ostrm;
    }

    inline std::ostream& operator<<(std::ostream& ostrm, const Embedding& embedding) {
        ostrm << "Embedding[";
        for (const auto& f: embedding) {
            ostrm << f << ", ";
        }
        return ostrm << "]";
    }


    struct ModelOverrides {
        std::optional<std::string> model_name;
        std::optional<float> top_p;
        std::optional<float> temperature;
        std::vector<std::string> stop_words;
    };

    enum MessageRole {
        kUnknown = 0,
        kAsisstant,
        kHuman,
        kSystem,
        kFunction,
        kTool
    };

    using MessageRoleNameMapping = std::unordered_map<MessageRole, std::string>;

    /**
     * Default role name mapping. This follows convention of OpenAI API
     */
    static MessageRoleNameMapping DEFAULT_ROLE_NAME_MAPPING =  {
        {kAsisstant, "assistant"},
        {kHuman, "user"},
        {kSystem, "system"},
        {kFunction, "function"},
        {kTool, "tool"}
    };

    using ChatPrompTeplateLiterals = std::initializer_list<std::pair<MessageRole, std::string>>;

    static PromptValue CreatePromptValue(ChatPrompTeplateLiterals&& literals, const MessageRoleNameMapping& mapping = DEFAULT_ROLE_NAME_MAPPING) {
        PromptValue pv;
        const auto chat_prompt_value = pv.mutable_chat();
        for(const auto& [role,content]: literals) {
            auto* msg = chat_prompt_value->add_messages();
            msg->set_role(mapping.at(role));
            msg->set_content(content);
        }
        return pv;
    }

    /**
     * Render function tool descriptions with given schema.
     * @tparam T
     * @param tools
     * @param with_args
     * @return
     */
    template<typename T>
    requires RangeOf<T, FunctionTool>
    static std::string RenderFunctionTools(T&& tools, const bool with_args = true) {
        auto fn_desc_view = tools | std::views::transform([&](const FunctionTool& fn_schema) {
            if (with_args) {
                return fmt::format("{}: {} arg={}", fn_schema.name(), fn_schema.description(), ProtobufUtils::Serialize(fn_schema.parameters()));
            }
            return fmt::format("{}: {}", fn_schema.name(), fn_schema.description());
        });
        return StringUtils::JoinWith(fn_desc_view, "\n");
    }


    namespace details {
        static std::string generate_next_object_id(const std::string_view& prefix) {
            static SnowflakeIDGenerator<1534832906275L> generator;
            return fmt::format("{}_{}", prefix, generator.NextID());
        }
    }

    class trace_span {
        std::string function_;
        u_int64_t start_;
    public:
        explicit trace_span(std::string function)
            : function_(std::move(function)), start_(ChronoUtils::GetCurrentTimeMillis()) {
            LOG_DEBUG("{} started", function_);
        }

        ~trace_span() {
            LOG_DEBUG("{} ended. duration {}ms", function_, ChronoUtils::GetCurrentTimeMillis() - start_);
        }
    };


}


#endif //MODELGLOBALS_H
