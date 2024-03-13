//
// Created by RobinQu on 2024/3/11.
//

#ifndef BASECHAIN_HPP
#define BASECHAIN_HPP

#include "IChain.hpp"
#include "IChainContextAware.hpp"
#include "LLMGlobals.hpp"

namespace INSTINCT_LLM_NS {
    struct ChainOptions {
        std::vector<std::string> input_keys {DEFAULT_PROMPT_INPUT_KEY};
        std::vector<std::string> output_keys  {DEFAULT_ANSWER_OUTPUT_KEY};
    };

    template<typename Output, typename Options=ChainOptions>
    requires std::derived_from<Options, ChainOptions>
    class BaseChain: public IChain<Output>, public IChainContextAware {
        Options options_;
    public:
        explicit BaseChain(Options options = {})
            : options_(std::move(options)) {
        }

        const Options& GetOptions() {
            return options_;
        }

        Output Invoke(const ContextPtr& input) override = 0;

        std::shared_ptr<ResultIterator<Output>> Batch(const std::vector<ContextPtr>& input) override {
            auto view = input | std::views::transform([&](const auto& context) {
                // auto copy = ContextMutataor::CreateCopyOf(context);
                return Invoke(context);
            });
            return create_result_itr_from_range(view);
        }

        std::shared_ptr<ResultIterator<Output>> Stream(const ContextPtr& input) override {
            // std::vector output = {std::move(Invoke(input))};
            // return create_result_itr_from_range(output);
            return {};
        }

        std::vector<std::string> GetInputKeys() override {
            return options_.input_keys;
        }

        std::vector<std::string> GetOutputKeys() override {
            return options_.output_keys;
        }
    };

    template<typename T, typename Options = ChainOptions>
    using ChainPtr = std::shared_ptr<BaseChain<T, Options>>;


    /**
     * Shared pointer to a TextChain
     */
    using TextChainPtr = ChainPtr<std::string>;

    /**
     * A Chain that generates a multi-line text result
     */
    using MultilineTextChainPtr = ChainPtr<std::vector<std::string>>;


}

#endif //BASECHAIN_HPP
