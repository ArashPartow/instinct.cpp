//
// Created by RobinQu on 2024/4/9.
//
#include <gtest/gtest.h>


#include <instinct/toolkit/builtin/serp_api.hpp>

namespace INSTINCT_AGENT_NS {
    using namespace INSTINCT_LLM_NS;

    class TestSerpAPI: public testing::Test {
    protected:
        void SetUp() override {
            SetupLogging();
            serp_api = CreateSerpAPI();
        }

        std::string Query(const std::string& text) {
            ToolCallObject invocation;
            SearchToolRequest search_tool_request;
            search_tool_request.set_query(text);
            invocation.mutable_function()->set_arguments(ProtobufUtils::Serialize(search_tool_request));
            const auto result = serp_api->Invoke(invocation);
            assert_true(!result.has_error());
            return result.return_value();
        }

        FunctionToolPtr serp_api;
    };

    TEST_F(TestSerpAPI, GetSchema) {
        const auto schema = serp_api->GetSchema();
        LOG_INFO(">> {}", schema.ShortDebugString());
        // only required fields are included in schema
        ASSERT_EQ(schema.parameters().properties_size(), 1);
        ASSERT_EQ(schema.parameters().properties().at("query").type(), "string");
//        ASSERT_EQ(schema.parameters().properties().at("result_limit").type(), "integer");
//        ASSERT_EQ(schema.parameters().properties().at("result_offset").type(), "integer");
    }

    TEST_F(TestSerpAPI, SimpleQuery) {
        const auto r1 = Query("how many oceans in the world?");
        LOG_INFO(">> {}", r1);

        const auto r2 = Query("what's PI?");
        LOG_INFO(">> {}", r2);

    }


}