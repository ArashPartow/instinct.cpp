//
// Created by RobinQu on 2024/3/7.
//

#ifndef LLMCHAIN_HPP
#define LLMCHAIN_HPP

#include <instinct/llm.pb.h>

#include <utility>

#include <instinct/chain/message_chain.hpp>
#include <instinct/llm_global.hpp>
#include <instinct/model/language_model.hpp>
#include <instinct/llm/base_llm.hpp>
#include <instinct/chat_model/base_chat_model.hpp>
#include <instinct/output_parser/base_output_parser.hpp>
#include <instinct/prompt/prompt_template.hpp>
#include <instinct/memory/chat_memory.hpp>
#include <instinct/tools/assertions.hpp>
#include <instinct/output_parser/multiline_generation_output_parser.hpp>
#include <instinct/input_parser/prompt_value_variant_input_parser.hpp>
#include <instinct/output_parser/string_output_parser.hpp>
#include <instinct/prompt/plain_prompt_template.hpp>
#include <instinct/functional/xn.hpp>
#include <instinct/core_global.hpp>


namespace INSTINCT_LLM_NS {
    using namespace INSTINCT_CORE_NS;

    // to adapt both LLM and ChatModel
    using LanguageModelVariant = std::variant<LLMPtr, ChatModelPtr>;

    /**
     * Create a simple MessageChain with given components
     * @tparam Input Input type
     * @tparam Output Output type
     * @param input_parser Paser for given input to context object
     * @param prompt_template PromptTemplate to generate prompts
     * @param model LLM or ChatModel
     * @param output_parser Output parser for model output
     * @param chat_memory IChatMemory to save and load memory
     * @param options Chain options
     * @return
     */
    template<typename Input,typename Output>
    static MessageChainPtr<Input,Output> CreateLLMChain(
            const InputParserPtr<Input>& input_parser,
            const LanguageModelVariant& model,
            const OutputParserPtr<Output>& output_parser,
            PromptTemplatePtr prompt_template = nullptr,
            const ChatMemoryPtr& chat_memory = nullptr,
            const ChainOptions& options = {}
            ) {
        if(!prompt_template) {
            prompt_template = CreatePlainPromptTemplate("{ " +  DEFAULT_QUESTION_INPUT_OUTPUT_KEY + " }");
        }

        StepFunctionPtr model_function;
        if (std::holds_alternative<LLMPtr>(model)) {
            model_function = std::get<LLMPtr>(model)->AsModelFunction();
        }
        if (std::holds_alternative<ChatModelPtr>(model)) {
            model_function = std::get<ChatModelPtr>(model)->AsModelFunction();
        }
        assert_true(model_function, "should contain model function");

        StepFunctionPtr step_function;
        if (chat_memory) {
            auto context_fn = xn::steps::mapping({
                {"format_instruction", output_parser->AsInstructorFunction()},
                {"chat_history", chat_memory->AsLoadMemoryFunction()},
                {"question", xn::steps::passthrough()}
            });
            step_function =  xn::steps::mapping({
              {"answer", context_fn | prompt_template | model_function},
              {"question", xn::steps::selection("question")}
            })
            | chat_memory->AsSaveMemoryFunction({.is_question_string=true, .prompt_variable_key="question", .answer_variable_key="answer"})
            | xn::steps::selection("answer");

        } else {
            step_function = xn::steps::mapping(
                    {
                       {"format_instruction",
                               output_parser->AsInstructorFunction()},
                       {"question",           xn::steps::passthrough()}

                    })
            | prompt_template
            | model_function;
        }

        return CreateFunctionalChain(
                input_parser,
                output_parser,
                step_function,
                options
        );
    }


    static MultilineChainPtr CreateMultilineChain(
            const LanguageModelVariant &model,
            const PromptTemplatePtr& prompt_template = nullptr,
            const ChainOptions& options = {},
            const ChatMemoryPtr& chat_memory = nullptr,
            InputParserPtr<PromptValueVariant> input_parser = nullptr,
            OutputParserPtr<MultilineGeneration> output_parser = nullptr
            ) {
        if(!input_parser) {
            input_parser = std::make_shared<PromptValueVariantInputParser>();
        }
        if (!output_parser) {
            output_parser = std::make_shared<MultilineGenerationOutputParser>();
        }
        return CreateLLMChain<PromptValueVariant, MultilineGeneration> (
                input_parser,
                model,
                output_parser,
                prompt_template,
                chat_memory,
                options
                );
    }


    static TextChainPtr CreateTextChain(
            const LanguageModelVariant &model,
            const PromptTemplatePtr& prompt_template = nullptr,
            const ChainOptions& options = {},
            const ChatMemoryPtr& chat_memory = nullptr,
            InputParserPtr<PromptValueVariant> input_parser = nullptr,
            OutputParserPtr<std::string> output_parser = nullptr
    ) {
        if(!input_parser) {
            input_parser = std::make_shared<PromptValueVariantInputParser>();
        }
        if (!output_parser) {
            output_parser = std::make_shared<StringOutputParser>();
        }


        return CreateLLMChain<PromptValueVariant, std::string>(
            input_parser,
            model,
            output_parser,
            prompt_template,
            chat_memory,
            options
        );
    }


}


#endif //LLMCHAIN_HPP
