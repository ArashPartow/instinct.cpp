//
// Created by RobinQu on 3/21/24.
//

#ifndef INSTINCT_IINPUTPARSER_HPP
#define INSTINCT_IINPUTPARSER_HPP

#include "LLMGlobals.hpp"
#include "functional/JSONContextPolicy.hpp"

namespace INSTINCT_LLM_NS {
    using namespace INSTINCT_CORE_NS;
    template<typename T>
    class IInputParser {
    public:
        virtual ~IInputParser() = default;

        virtual JSONContextPtr ParseInput(const T& input) = 0;
    };
}

#endif //INSTINCT_IINPUTPARSER_HPP