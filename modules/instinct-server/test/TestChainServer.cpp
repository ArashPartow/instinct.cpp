//
// Created by RobinQu on 3/19/24.
//
#include <gtest/gtest.h>
#include <instinct/chain/LLMChain.hpp>
#include <instinct/chat_model/OpenAIChat.hpp>
#include <instinct/LLMTestGlobals.hpp>
#include <instinct/endpoint/chain/MultiChainController.hpp>
#include <instinct/server/httplib/HttpLibServer.hpp>

namespace INSTINCT_SERVER_NS {
    using namespace INSTINCT_LLM_NS;
    using namespace INSTINCT_CORE_NS;

    class PlainChainServerTest : public ::testing::Test {

    protected:
        void SetUp() override {
            SetupLogging();
            auto llm = CreateOpenAIChatModel(INSTINCT_LLM_NS::DEFAULT_NITRO_SERVER_CONFIGURATION);
            chain1_ = CreateTextChain(llm);
        }
        TextChainPtr chain1_;
    };


    TEST_F(PlainChainServerTest, Lifecycle) {
        HttpLibServer server({.port = 9999});
        const auto controller = CreateMultiChainController();
        controller->AddNamedChain("chain1", chain1_->GetStepFunction());
        server.Bind();
        server.Shutdown();
    }
}

