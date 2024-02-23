//
// Created by RobinQu on 2024/1/12.
//

#ifndef LLMRESULT_H
#define LLMRESULT_H
#include <vector>

#include "Generation.hpp"
#include "prompt/Forwards.hpp"


LC_CORE_NS {

class LLMResult {
public:
    std::vector<std::vector<GenerationVariant>> generations;
    OptionDict llm_output;

    LLMResult() = default;

    LLMResult(std::vector<std::vector<GenerationVariant>> generations, OptionDict llm_output)
        : generations(std::move(generations)),
          llm_output(std::move(llm_output)) {
    }


    std::vector<Generation> flatten();
};

using LLMResultPtr = std::shared_ptr<LLMResult>;

} // core
// langchain

#endif //LLMRESULT_H