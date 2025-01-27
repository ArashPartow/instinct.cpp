//
// Created by RobinQu on 2024/4/9.
//

#ifndef LLMMATH_HPP
#define LLMMATH_HPP

#include <instinct/agent.pb.h>
#include <exprtk.hpp>

#include <instinct/chain/llm_chain.hpp>
#include <instinct/chat_model/base_chat_model.hpp>
#include <instinct/prompt/plain_chat_prompt_template.hpp>
#include <instinct/prompt/plain_prompt_template.hpp>
#include <instinct/toolkit/proto_message_function_tool.hpp>



namespace INSTINCT_LLM_NS {
    /**
     * Prompt LLM to generate math expression and then cacluate the result of expression using `exprtk` math library.
     *
     * Model selection notice: some model includling `llama2:7b` series performs badlly for generating expressions. Tested working models:
     *
     * Opensourced models:
     *
     * * General-purpose: `mixtral:8x7b`, `starling-lm:7b`, `mistral:7b`.
     *
     * * Tuned model for math: `zephyr-beta-math-Mistral-7B-Instruct-v0.2-slerp`
     *
     * Comercial models: `gpt-3.5-turbo-instruct` and other OpenAI GPT models.
     *
     * Other recommendations:
     *
     * * set model's `temperature` to zero.
     *
     * * increase tool's `max_attemps`.
     *
     *
     */
    class LLMMath final: public ProtoMessageFunctionTool<CaculatorToolRequest, CalculatorToolResponse> {
        ChatModelPtr chat_model_;
        PromptTemplatePtr prompt_template_;
        TextChainPtr chain_;

    public:
        explicit LLMMath(ChatModelPtr chat_model, PromptTemplatePtr prompt_template = nullptr, const FunctionToolOptions& options ={}):
            ProtoMessageFunctionTool("Calculator", "Use this tool when you need to answer questions about math.", options) ,
            chat_model_(std::move(chat_model)),
            prompt_template_(std::move(prompt_template))
        {
            if(!prompt_template_) {
                prompt_template_ = CreatePlainPromptTemplate(R""(
Translate a math problem into a expression that can be evaluated by a math library. Please don't calculate result directly.

Use the following format:

Question: What is 37593 * 67?
Math expression: 37593 * 67

Question: 37593^(1/5)
Math expression: 37593**(1/5)

Please reply in single line and following instructions above.

Question: {question})"");
            }
            chain_ = CreateTextChain(chat_model_, prompt_template_);
        }

        CalculatorToolResponse DoExecute(const CaculatorToolRequest &input) override {
            static auto MATH_EXPRESSION_REGEX = std::regex{R"(Math expression:\s*(.*)\n?)"};
            auto answer_string = chain_->Invoke(input.math_question());
            LOG_DEBUG("model output: {}", answer_string);
            if (const auto matches = StringUtils::MatchPattern(answer_string, MATH_EXPRESSION_REGEX); !matches.empty() && matches.front().size() == 2) {
                // use first match as final answer
                const auto v = Evaulate_<double>(matches.front().str(1));
                CalculatorToolResponse response;
                response.set_answer(v);
                LOG_DEBUG("math evaluator output: {}", v);
                return response;
            }
            throw InstinctException("Malformed output for LLMMath");
        }

        FunctionToolSelfCheckResponse SelfCheck() override {
            CaculatorToolRequest request;
            request.set_math_question("what's result of 1+1?");
            const auto resp = DoExecute(request);
            FunctionToolSelfCheckResponse response;
            response.set_passed(resp.answer() == 2);
            return response;
        }

    private:
        template <typename T>
        T Evaulate_(const std::string& expression_string) {
            typedef exprtk::symbol_table<T> symbol_table_t;
            typedef exprtk::expression<T>   expression_t;
            typedef exprtk::parser<T>       parser_t;
            symbol_table_t symbol_table;
            parser_t parser;
            return parser.compile(expression_string,symbol_table);
        }
    };

    static FunctionToolPtr CreateLLMMath(const ChatModelPtr& chat_model, const FunctionToolOptions& options ={}, const PromptTemplatePtr& prompt_template = nullptr) {
        return std::make_shared<LLMMath>(chat_model, prompt_template, options);
    }
}


#endif //LLMMATH_HPP
