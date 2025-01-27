//
// Created by RobinQu on 2024/6/17.
//

#include <gtest/gtest.h>
#include <instinct/embedding_model/local_embedding_model.hpp>
#include <instinct/tools/tensor_utils.hpp>


namespace INSTINCT_LLM_NS {


    class LocalEmbeddingModelTest: public testing::Test {
    protected:
        void SetUp() override {
            SetupLogging();
            PreloadEmbeddingModelFiles();
        }
    };


    TEST_F(LocalEmbeddingModelTest, BGE_M3_EMBEDDING) {
        const auto embedding_model = CreateLocalEmbeddingModel(ModelType::BGE_M3_EMBEDDING);
        ASSERT_EQ(embedding_model->GetDimension(), 1024);
        const auto v1 = embedding_model->EmbedQuery("hello world");
        TensorUtils::PrintEmbedding(v1);
    }




}
