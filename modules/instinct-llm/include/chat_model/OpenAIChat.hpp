//
// Created by RobinQu on 2024/3/15.
//

#ifndef OPENAICHAT_HPP
#define OPENAICHAT_HPP

#include "BaseChatModel.hpp"
#include "LLMGlobals.hpp"
#include "commons/OpenAICommons.hpp"
#include "tools/HttpRestClient.hpp"
#include <llm.pb.h>

#include "tool/FunctionToolUtils.hpp"

namespace INSTINCT_LLM_NS {


    /**
    * OpenAI API endpoint reference:
    * https://platform.openai.com/docs/api-reference/chat/create
    */
    class OpenAIChat final: public BaseChatModel {
        OpenAIConfiguration configuration_;
        HttpRestClient client_;
        std::vector<OpenAIChatCompletionRequest_ChatCompletionTool> function_tools_;
    public:
        explicit OpenAIChat(OpenAIConfiguration configuration)
            : BaseChatModel(configuration.base_options), configuration_(std::move(configuration)), client_(configuration_.endpoint) {
            client_.GetDefaultHeaders().emplace("Authorization", fmt::format("Bearer {}", GetAPIKey_()));
        }

        void BindToolSchemas(const std::vector<FunctionTool> &function_tool_schema) override {
            for(const auto& function_tool: function_tool_schema) {
                OpenAIChatCompletionRequest_ChatCompletionTool tool;
                tool.set_type("function");
                tool.mutable_function()->CopyFrom(function_tool);
                function_tools_.push_back(tool);
            }
        }

        void CallOpenAI(const MessageList& message_list, BatchedLangaugeModelResult& batched_language_model_result) {
            const auto req = BuildRequest_(message_list, false);
            const auto resp = client_.PostObject<OpenAIChatCompletionRequest, OpenAIChatCompletionResponse>(DEFAULT_OPENAI_CHAT_COMPLETION_ENDPOINT, req);

            auto* language_model_result = batched_language_model_result.add_generations();
            for(const auto& choice: resp.choices()) {
                auto* single_result = language_model_result->add_generations();
                single_result->set_text(choice.message().content());
                single_result->set_is_chunk(false);
                single_result->mutable_message()->CopyFrom(choice.message());
            }
        }

        BatchedLangaugeModelResult Generate(const std::vector<MessageList>& message_matrix) override {
            // TODO make it parelle
            BatchedLangaugeModelResult batched_langauge_model_result;
            for (const auto& mesage_list: message_matrix) {
                CallOpenAI(mesage_list, batched_langauge_model_result);
            }
            return batched_langauge_model_result;
        }

        AsyncIterator<LangaugeModelResult> StreamGenerate(const MessageList& messages) override {
            const auto req = BuildRequest_(messages, true);
            const auto chunk_itr = client_.StreamChunkObject<OpenAIChatCompletionRequest, OpenAIChatCompletionChunk>(DEFAULT_OPENAI_CHAT_COMPLETION_ENDPOINT, req, true, {"[DONE]"});
            return chunk_itr | rpp::operators::map([](const OpenAIChatCompletionChunk& chunk) {
                LangaugeModelResult langauge_model_result;
                for (const auto& choice: chunk.choices()) {
                    auto* single_result = langauge_model_result.add_generations();
                    single_result->set_text(choice.delta().content());
                    single_result->set_is_chunk(false);
                    single_result->mutable_message()->CopyFrom(choice.delta());
                }
                return langauge_model_result;
            });
        }

    private:
        OpenAIChatCompletionRequest BuildRequest_(const MessageList& message_list, const bool stream) {
            OpenAIChatCompletionRequest req;
            for (const auto& msg: message_list.messages()) {
                req.add_messages()->CopyFrom(msg);
            }
            req.set_model(configuration_.model_name);
            req.set_n(1);
            req.set_seed(configuration_.seed);
            req.set_temperature(configuration_.temperature);
            req.set_max_tokens(configuration_.max_tokens);
            if (configuration_.json_object) {
                req.mutable_response_format()->set_type("json_object");
            }
            req.mutable_tools()->Add(function_tools_.begin(), function_tools_.end());
            req.set_stream(stream);
            return req;
        }

        std::string GetAPIKey_() const {
            if (StringUtils::IsNotBlankString(configuration_.api_key)) {
                return configuration_.api_key;
            }
            if (const auto api_key_env = SystemUtils::GetEnv("OPENAI_API_KEY"); StringUtils::IsNotBlankString(api_key_env)) {
                return api_key_env;
            }
            LOG_WARN("API key for OpenAI is not found in configuration or envrionment variables.");
            // won't thorw as some local LLMs don't need an API key for authentication at all
            return "";
        }

    };

    static ChatModelPtr CreateOpenAIChatModel(const OpenAIConfiguration& configuration = {}) {
        return std::make_shared<OpenAIChat>(configuration);
    }
}


#endif //OPENAICHAT_HPP
