//
// Created by RobinQu on 2024/3/3.
//

#include <gtest/gtest.h>
#include <instinct/core_global.hpp>
#include <instinct/tokenizer/gpt2_bpe_file_reader.hpp>

namespace INSTINCT_LLM_NS {
    TEST(GPT2BPEFileReader, TestLoad) {
        const std::filesystem::path assets_dir = std::filesystem::current_path() / "_assets";
        GPT2BPEFileReader file_reader(
            assets_dir / "bpe_ranks/vocab.bpe",
            assets_dir / "bpe_ranks/encoder.json"
            );
        auto bpe_ranks = file_reader.Fetch();
        std::cout <<"item_count=" <<  bpe_ranks.size() << std::endl;
        ASSERT_EQ(bpe_ranks.size(), 50256);
    }
}
