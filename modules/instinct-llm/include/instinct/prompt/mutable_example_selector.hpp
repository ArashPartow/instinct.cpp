//
// Created by RobinQu on 2024/2/14.
//

#ifndef MUTABLEEXAMPLESELECTOR_H
#define MUTABLEEXAMPLESELECTOR_H

#include <instinct/llm_global.hpp>
#include <instinct/prompt/example_selector.hpp>
#include <instinct/prompt/plain_prompt_template.hpp>

namespace INSTINCT_LLM_NS {
    using namespace INSTINCT_CORE_NS;

    class MutableExampleSelector : public IExampleSelector {
    protected:
        PromptExamples examples_;
        PromptTemplatePtr example_prompt_template_;

    public:
        explicit MutableExampleSelector(PromptTemplatePtr example_prompt_template)
            : examples_(),
              example_prompt_template_(std::move(example_prompt_template)) {
        }

        void AddExample(const PromptExample& example) override {
            examples_.push_back(example);
        }

        [[nodiscard]] const PromptExamples& GetAllExamples() override {
            return examples_;
        }

    };



} // namespace INSTINCT_CORE_NS

#endif //MUTABLEEXAMPLESELECTOR_H
