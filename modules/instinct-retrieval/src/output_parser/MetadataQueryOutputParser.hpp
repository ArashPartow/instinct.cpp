//
// Created by RobinQu on 2024/3/12.
//

#ifndef METADATAQUERYOUTPUTPARSER_HPP
#define METADATAQUERYOUTPUTPARSER_HPP

#include "output_parser/BaseOutputParser.hpp"
#include "RetrievalGlobals.hpp"

namespace INSTINCT_RETRIEVAL_NS {
    using namespace INSTINCT_LLM_NS;

    class MetadataQueryOutputParser final: public BaseOutputParser<MetadataQuery> {

        MetadataSchema metadata_schema_;

    public:
        explicit MetadataQueryOutputParser(const MetadataSchema& metadata_schema)
            : metadata_schema_(std::move(metadata_schema)) {
        }

        MetadataQuery ParseResult(const Generation& result) override;

        std::string GetFormatInstruction() override;
    };
}


#endif //METADATAQUERYOUTPUTPARSER_HPP