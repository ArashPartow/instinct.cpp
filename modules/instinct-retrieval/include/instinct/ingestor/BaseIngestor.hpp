//
// Created by RobinQu on 2024/3/13.
//

#ifndef BASEINGESTOR_HPP
#define BASEINGESTOR_HPP


#include <instinct/ingestor/IIngestor.hpp>
#include <instinct/tools/document_utils.hpp>

namespace INSTINCT_RETRIEVAL_NS {
    using namespace U_ICU_NAMESPACE;

    using DocumentPostProcessor = std::function<void(Document& doc)>;

    class BaseIngestor: public IIngestor {
        DocumentPostProcessor document_post_processor_;
    public:
        explicit BaseIngestor(DocumentPostProcessor document_post_processor)
            : document_post_processor_(std::move(document_post_processor)) {
        }

        AsyncIterator<Document> Load() override = 0;

        [[nodiscard]] Document CreateNewDocument(
            const std::string& text,
            const std::string& parent_doc_id,
            const int page_no,
            const std::string& source
        ) const {
            Document document;
            document.set_text(text);
            DocumentUtils::AddPresetMetadataFields(document, parent_doc_id, page_no, source);
            if (document_post_processor_) {
                document_post_processor_(document);
            }
            return document;
        }

        AsyncIterator<Document> LoadWithSplitter(const TextSplitterPtr& text_splitter) override {
            const auto docs_itr = Load();
            return text_splitter->SplitDocuments(docs_itr);
        }
    };

    using IngestorPtr = std::shared_ptr<BaseIngestor>;
}

#endif //BASEINGESTOR_HPP
