//
// Created by RobinQu on 2024/5/11.
//

#ifndef LLMCOMPILERPLANER_HPP
#define LLMCOMPILERPLANER_HPP
#include <instinct/agent/patterns/llm_compiler/llm_compiler_planer_agent_state_input_parser.hpp>
#include <instinct/agent/patterns/llm_compiler/llm_compiler_planer_thought_output_parser.hpp>
#include <instinct/llm_global.hpp>
#include <instinct/agent/executor/agent_executor.hpp>
#include <instinct/chat_model/base_chat_model.hpp>
#include <instinct/prompt/plain_chat_prompt_template.hpp>


namespace INSTINCT_LLM_NS {

    /**
     * named context variables are:
     * 1. question: user input
     * 2. num_tools: number of tools
     * 3. tool_descriptions: formated list of tool descriptions
     * 4. replan_instruction: re-planing instruction if applicable
     * 5. context: context for re-planing if applicable
     *
     * Implementations:
     * 1. if last step doesn't exist, then let's do first plan
     * 2. if last step has observation (except join), we do join
     * 2.1 if `join` gives out final result, we return thought with final message
     * 2.2 if `join` gives out replan request, we run LLM with replan prompt.
    */
    static PlannerPtr CreateLLMCompilerPlaner(
        const ChatModelPtr &chat_model,
        const LLMCompilerPlanerAgentStateInputParserOptions& input_parser_options = {},
        const LLMCompilerPlanerThoughtOutputParserOptions& output_parse_options = {},
        PromptTemplatePtr prompt_template = nullptr
    ) {
        if (!prompt_template) {
            prompt_template = CreatePlainChatPromptTemplate({
                {
                kHuman,
                R"(Given a user query, create a plan to solve it with the utmost parallelization. Each plan should comprise an action from the following {num_tools} types:
{tool_descriptions}
{num_tools}. join: Collects and combines results from prior actions. No arguments needed.

Guidelines:
- Each action MUST have a unique ID, which is strictly increasing.
- Ensure the plan maximizes parallelization.
- Respond with the task list and each task takes one and only one line in the following format: ID. action_name(input arguments in JSON format, without newline).
- If inputs for actions are outputs from preceding actions,  always use the format $ID to denote the numeric ID of the previous action whose output will be used as the input.
- Only use the provided action types. Never use action not listed above. If a query cannot be addressed using these, invoke the join action for the next steps.
- Never introduce new actions other than the ones provided.
- Each action described above contains input/output types and description.
- You must strictly adhere to the input and output types for each action.
- The action descriptions contain the guidelines. You MUST strictly follow those guidelines when you use the actions.
- Each action in the plan should strictly be one of the above types.
- An LLM agent is called upon invoking join() to either finalize the user query or wait until the plans are executed.
- join should always be the last action in the plan, and will be called in two scenarios:
    (a) if the answer can be determined by gathering the outputs from tasks to generate the final response.
    (b) if the answer cannot be determined in the planning phase before you execute the plans.
- Always call join as the last action in the plan only once. Say '<END_OF_PLAN>' after you call join in a new line.
{replan}

{examples}

Question: {question}

{context}
)"
                }
            });
        }

        const auto input_parser = CreateLLMCompilerPlanerAgentStateInputParser(input_parser_options);
        const auto output_parser = CreateLLMCompilerPlanerThoughtOutputParser(output_parse_options);

        // if tools are provided, use LLMCompiler's prompt template
        const auto tool_chain = prompt_template | chat_model->AsModelFunction();

        // chat_chain is used if no tools are provided
        const auto chat_template = CreatePlainChatPromptTemplate({{kHuman, "{question}"}});
        const auto chat_chain = chat_template | chat_model->AsModelFunction();
        const auto condition = [](const JSONContextPtr& context) {
            // `join` is always counted
            bool has_tools = context->RequireMappingData().at("num_tools")->RequirePrimitive<int>() > 1;
            JSONContextPtr ret = CreateJSONContext();
            ret->ProducePrimitive(has_tools);
            return ret;
        };
        return input_parser | xn::steps::branch(condition, tool_chain, chat_chain) | output_parser;


    }
}

#endif //LLMCOMPILERPLANER_HPP
