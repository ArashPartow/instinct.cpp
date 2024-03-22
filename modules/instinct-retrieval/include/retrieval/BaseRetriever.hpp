//
// Created by RobinQu on 2024/3/17.
//

#ifndef INSTINCT_BASERETRIEVER_HPP
#define INSTINCT_BASERETRIEVER_HPP

#include "retrieval/IRetriever.hpp"
#include "retrieval/DocumentUtils.hpp"

namespace INSTINCT_RETRIEVAL_NS {
    using namespace INSTINCT_LLM_NS;

    class BaseRetriever : public ITextRetriever {
    public:

        [[nodiscard]] AsyncIterator<Document> Retrieve(const TextQuery &query) const override = 0;

        [[nodiscard]] StepFunctionPtr AsContextRetrieverFunction(const RAGChainOptions& options) const {
            return std::make_shared<LambdaStepFunction>([&](const JSONContextPtr &ctx) {
                const auto question_string = ctx->RequirePrimitive<std::string>(options.condense_question_key);
                const auto doc_itr = Retrieve({.text = question_string, .top_k = options.top_k});
                std::string context_string = DocumentUtils::CombineDocuments(doc_itr);
                ctx->PutPrimitive(options.context_output_key, context_string);
                return ctx;
            }, std::vector{options.context_output_key}, std::vector {options.context_output_key});
        }

    };

    using RetrieverPtr = std::shared_ptr<BaseRetriever>;

    class BaseStatefulRetriever : public BaseRetriever, public IStatefulRetriever {
    public:
        [[nodiscard]] AsyncIterator<Document> Retrieve(const TextQuery &query) const override = 0;

        void Ingest(const AsyncIterator<Document> &input) override = 0;
    };

    using StatefulRetrieverPtr = std::shared_ptr<BaseStatefulRetriever>;

}


#endif //INSTINCT_BASERETRIEVER_HPP
