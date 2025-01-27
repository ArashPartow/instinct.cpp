//
// Created by RobinQu on 2024/5/14.
//

#ifndef LLMCOMPILERJOINERTASKGRAPHINPUTPARSER_HPP
#define LLMCOMPILERJOINERTASKGRAPHINPUTPARSER_HPP
#include <instinct/llm_global.hpp>
#include <instinct/agent/patterns/llm_compiler/task_graph_utils.hpp>
#include <instinct/input_parser/base_input_parser.hpp>

namespace INSTINCT_LLM_NS {
    struct LLMCompilerJoinerTaskGraphInputParserOptions {
        InputParserOptions base_options = {};
        std::string instructions;
    };

    class LLMCompilerJoinerTaskGraphInputParser final: public BaseInputParser<LLMCompilerTaskGraph> {
        LLMCompilerJoinerTaskGraphInputParserOptions options_;
    public:
        explicit LLMCompilerJoinerTaskGraphInputParser(const LLMCompilerJoinerTaskGraphInputParserOptions& options)
            : BaseInputParser<LLMCompilerTaskGraph>(options.base_options), options_(options) {
        }

        JSONContextPtr ParseInput(const LLMCompilerTaskGraph &graph) override {
            std::string scratchpad;
            TaskGraphUtils::BuildAgentScratchPad(graph, scratchpad);
            return CreateJSONContext({
                {"question", graph.question()},
                {"agent_scratchpad", scratchpad},
                // extra instructions controlled by user input
                {"instructions", options_.instructions},
            });
        }
    };

    static InputParserPtr<LLMCompilerTaskGraph> CreateLLMCompilerJoinerTaskGraphInputParser(const LLMCompilerJoinerTaskGraphInputParserOptions& options = {}) {
        return std::make_shared<LLMCompilerJoinerTaskGraphInputParser>(options);
    }

}
#endif //LLMCOMPILERJOINERTASKGRAPHINPUTPARSER_HPP
