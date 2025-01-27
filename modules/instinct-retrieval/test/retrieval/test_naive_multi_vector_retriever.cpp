//
// Created by RobinQu on 2024/3/13.
//


#include <gtest/gtest.h>

#include <instinct/llm_test_global.hpp>
#include <instinct/retriever_object_factory.hpp>
#include <instinct/chat_model/ollama_chat.hpp>
#include <instinct/ingestor/ingestor.hpp>
#include <instinct/ingestor/directory_tree_ingestor.hpp>
#include <instinct/retrieval/multi_query_retriever.hpp>
#include <instinct/retrieval/multi_vector_retriever.hpp>
#include <instinct/store/duckdb/duckdb_doc_store.hpp>
#include <instinct/store/duckdb/duckdb_vector_store.hpp>



namespace INSTINCT_RETRIEVAL_NS {
    using namespace INSTINCT_LLM_NS;


    class MultiVectorRetrieverTest: public testing::Test {
    protected:
        void SetUp() override {
            SetupLogging();
            auto root_path = ensure_random_temp_folder();
            std::cout << "MultiVectorRetrieverTest at " << root_path << std::endl;
            llm_ = create_pesudo_chat_model();

            size_t dimension = 4096;

            auto meta_schema = CreateVectorStorePresetMetadataSchema();

            EmbeddingsPtr embedding_model = create_pesudo_embedding_model(dimension);
            DuckDBStoreOptions db_options = {
                .table_name = "document_table",
                .db_file_path = root_path / "doc_store.db"
            };
            doc_store_ = CreateDuckDBDocStore(db_options);

            DuckDBStoreOptions summary_db_options = {
                .table_name = "summaries_table",
                .db_file_path = root_path / "summary_store.db",
                .dimension = dimension,
            };
            summary_store_ = CreateDuckDBVectorStore(embedding_model, summary_db_options, meta_schema);

            DuckDBStoreOptions query_db_options = {
                .table_name = "queries_table",
                .db_file_path = root_path / "query_store.db",
                .dimension = dimension
            };
            hypothetical_queries_store_ = CreateDuckDBVectorStore(embedding_model, query_db_options, meta_schema);

            // corpus data is copied to build tree in CMakeLists.txt
            asset_dir_ = std::filesystem::current_path() / "_corpus";

            // load all recipes in folder
            const auto recipes_dir = asset_dir_ / "recipes";
            std::cout << "reading recipes from " << recipes_dir << std::endl;
            recipes_ingestor_ = RetrieverObjectFactory::CreateDirectoryTreeIngestor(recipes_dir);
        }

        std::filesystem::path asset_dir_;
        ChatModelPtr llm_;
        DocStorePtr doc_store_;
        VectorStorePtr summary_store_;
        VectorStorePtr hypothetical_queries_store_;
        IngestorPtr recipes_ingestor_;
    };

    TEST_F(MultiVectorRetrieverTest, TestReteiveWithSummary) {
        auto retriever = CreateSummaryGuidedRetriever(
            llm_,
            doc_store_,
            summary_store_
        );
        retriever->Ingest(recipes_ingestor_->Load());

        const auto summary_count = summary_store_->CountDocuments();
        ASSERT_GT(summary_count, 0);

        // use simple search
        const auto doc_itr = retriever->Retrieve({.text = "Shortcake", .top_k = 2});

        auto doc_vec = CollectVector(doc_itr);
        ASSERT_EQ(doc_vec.size(), 2);
    }

    TEST_F(MultiVectorRetrieverTest, TestRetrieverWithHypotheticalQueries) {
        auto retriever = CreateHypotheticalQueriesGuidedRetriever(
                llm_,
                doc_store_,
                hypothetical_queries_store_
        );
        retriever->Ingest(recipes_ingestor_->Load());

        const auto queries_count = hypothetical_queries_store_->CountDocuments();
        ASSERT_GT(queries_count, 0);

        // simple search
        const auto doc_itr = retriever->Retrieve({.text = "Shortcake", .top_k = 2});

        auto doc_vec = CollectVector(doc_itr);
        // first recall may contain duplicated docs which can be filtered away
        ASSERT_TRUE(doc_vec.size() <= 2);
    }



}
