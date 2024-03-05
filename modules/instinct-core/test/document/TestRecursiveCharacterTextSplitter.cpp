//
// Created by RobinQu on 2024/3/4.
//
#include <gtest/gtest.h>


#include "CoreGlobals.hpp"
#include "document/RecursiveCharacterTextSplitter.hpp"
#include "document/tokenizer/TiktokenTokenizer.hpp"
#include "document/tokenizer/Tokenizer.hpp"

#include "Corpus.hpp"
#include "tools/Assertions.hpp"

namespace INSTINCT_CORE_NS {
    TEST(RecursiveCharacterTextSplitter, split_text_with_seperator) {
        auto results = details::split_text_with_seperator(
        u32_utils::copies_of(5, "abcdef"),
        "",true);
        ASSERT_EQ(results.size(), 30);
        ASSERT_EQ(results[29].char32At(0), UChar32{'f'});
    }

    TEST(RecursiveCharacterTextSplitter, TestSimpleSplit) {
        auto* text_splitter = new RecursiveCharacterTextSplitter({.chunk_size = 15});
        auto result = text_splitter->SplitText(u32_utils::copies_of(5, "abcdef"));
        u32_utils::print_splits("splits: ", result);
        ASSERT_TRUE(check_equality(result, std::vector {"abcdefabcdefabc", "defabcdefabcdef"}));
        delete text_splitter;

        text_splitter = new RecursiveCharacterTextSplitter({.chunk_size = 5});
        result = text_splitter->SplitText(corpus::text4);
        u32_utils::print_splits("splits: ", result);
        ASSERT_EQ(result.size(), 3);
        ASSERT_EQ(result[2].char32At(0), UChar32{U'👋'});
        delete text_splitter;
    }

    TEST(RecursiveCharacterTextSplitter, TestSplitWithNonEnglishText) {
        auto* text_splitter = new RecursiveCharacterTextSplitter({.chunk_size = 20});
        auto result = text_splitter->SplitText(corpus::text6);
        u32_utils::print_splits("splits: ", result);
        delete text_splitter;
    }

    TEST(RecursiveCharacterTextSplitter, TestSplitWithTokenizer) {
        const std::filesystem::path assets_dir =
                std::filesystem::current_path() / "./modules/instinct-core/test/_assets";
        auto* tokenizer =
                TiktokenTokenizer::MakeGPT4Tokenizer(assets_dir / "cl100k_base" / "cl100k_base.tiktoken");

        auto* text_splitter = RecursiveCharacterTextSplitter::FromTiktokenTokenizer(tokenizer, { .chunk_size = 20});
        auto splits = text_splitter->SplitText(corpus::text3);
        u32_utils::print_splits("splits: ", splits);

        delete tokenizer;
        delete text_splitter;
    }
}
