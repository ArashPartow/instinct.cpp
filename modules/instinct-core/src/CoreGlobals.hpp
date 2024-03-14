//
// Created by RobinQu on 2024/2/12.
//

#ifndef COREGLOBALS_H
#define COREGLOBALS_H

#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <unicode/unistr.h>
#include <unicode/uversion.h>
#include <google/protobuf/message.h>
#include <fmtlog-inl.h>


#define INSTINCT_CORE_NS instinct::core

// another set of marcos to hide log implementation
#define LOG_ERROR loge
#define LOG_WARN logw
#define LOG_INFO logi
#define LOG_DEBUG logd

namespace INSTINCT_CORE_NS {

    static void SetupLogging() {
        // TODO setup coloring for each level
        fmtlog::setLogLevel(fmtlog::DBG);
        fmtlog::startPollingThread();
    }

    template<class>
    inline constexpr bool always_false_v = false;

    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };
    // explicit deduction guide (not needed as of C++20)
    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;

    template <typename R, typename V>
        concept RangeOf = std::ranges::range<R> && std::same_as<std::ranges::range_value_t<R>, V>;

    using U32String = U_ICU_NAMESPACE::UnicodeString;

    template <typename T>
    concept is_pb_message = std::derived_from<T, google::protobuf::Message>;


    class InstinctException: public std::runtime_error {
        std::string reason_;

    public:
        explicit InstinctException(const std::string& basic_string)
            : std::runtime_error(basic_string), reason_(basic_string) {
        }

        explicit InstinctException(const char* const string)
            : std::runtime_error(string), reason_(string) {
        }

        explicit InstinctException(const runtime_error& runtime_error, std::string  basic_string)
            : std::runtime_error(runtime_error),reason_(std::move(basic_string)) {

        }

        [[nodiscard]] const char* what() const noexcept override {
            return reason_.data();
        }
    };

}


#endif //COREGLOBALS_H
