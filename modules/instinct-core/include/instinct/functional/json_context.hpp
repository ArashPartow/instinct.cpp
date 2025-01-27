//
// Created by RobinQu on 3/21/24.
//

#ifndef INSTINCT_JSONCONTEXTPOLICY_HPP
#define INSTINCT_JSONCONTEXTPOLICY_HPP

#include <instinct/core_global.hpp>
#include <instinct/functional/context.hpp>
#include <google/protobuf/util/json_util.h>

#include <instinct/tools/assertions.hpp>

namespace INSTINCT_CORE_NS {
    using namespace google::protobuf;

    static const std::string MESSAGE_WRAPPER_DATA_KEY = "__protobuf_message__";

    static const std::string MAPPING_DATA_WRAPPER_DATA_KEY = "__mapping_data__";


    using JSONObject = nlohmann::json;

    class JSONContextPolicy;
    using JSONContextPtr = ContextPtr<JSONContextPolicy>;
    using JSONMappingContext = std::unordered_map<std::string, JSONContextPtr>;

    class JSONContextPolicy {
        JSONObject data_;
    public:
        using ValueType = JSONObject;

        explicit JSONContextPolicy(JSONObject data = {}) : data_(std::move(data)) {}

        template<typename T>
        T RequirePrimitive() const {
            assert_true(IsPrimitive(), "expecting a primitive value");
            return data_.get<T>();
        }

        template<typename T>
        void PutValue(T&& value) {
            data_ = std::forward<T>(value);
        }

        [[nodiscard]] const ValueType& GetValue() const {
            return data_;
        }

        template<typename T>
        T RequireMessage() const {
            // TODO use reflection instead
            assert_true(IsMessage(), "expecting a message wrapper type");
            T result;
            auto status = util::JsonStringToMessage(data_.at(MESSAGE_WRAPPER_DATA_KEY).template get<std::string>(), &result);
            assert_true(status.ok(), "message deserialization failed: " + std::string(status.message()));
            return result;
        }

        template<typename T>
        void PutMessage(const T& message) {
            // TODO use reflection instead
            std::string buf;
            auto status = util::MessageToJsonString(message, &buf);
            assert_true(status.ok(), "message serialization failed: " + std::string(status.message()));
            data_ = nlohmann::json{{MESSAGE_WRAPPER_DATA_KEY, buf}};
        }

        [[nodiscard]] bool IsPrimitive() const {
            return data_.is_primitive();
        }

        [[nodiscard]] bool IsMessage() const {
            return data_.is_object() && data_.contains(MESSAGE_WRAPPER_DATA_KEY);
        }

        [[nodiscard]] bool IsMappingObject() const {
            return data_.is_object() && data_.contains(MAPPING_DATA_WRAPPER_DATA_KEY);
        }

        void PutMappingObject(const JSONMappingContext& mapping_data) {
            JSONObject  new_obj;
            for (const auto& [k,v]: mapping_data) {
                new_obj[k] = v->GetValue();
            }
            data_ = nlohmann::json{{MAPPING_DATA_WRAPPER_DATA_KEY, new_obj}};
        }

        [[nodiscard]] JSONMappingContext GetMappingObject() const {
            assert_true(IsMappingObject(), "expecting MappingObject wrapper format");
            auto map_value = data_[MAPPING_DATA_WRAPPER_DATA_KEY];
            JSONMappingContext mapping_data;
            for(const auto&[k,v]: map_value.items()) {
                JSONContextPolicy policy {v};
                mapping_data[k] = std::make_shared<IContext<JSONContextPolicy>>(policy);
            }
            return mapping_data;
        }

    };

    static JSONContextPtr CreateJSONContext(const Message& message) {
        JSONContextPolicy policy;
        auto ctx = std::make_shared<IContext<JSONContextPolicy>>(policy);
        ctx->ProduceMessage(message);
        return ctx;
    }

    static JSONContextPtr CreateJSONContext(const nlohmann::json& data = nlohmann::json::parse("{}")) {
        JSONContextPolicy policy;
        auto ctx = std::make_shared<IContext<JSONContextPolicy>>(policy);
        if (data.is_string()) {
            ctx->ProducePrimitive(data.get<std::string>());
        }
        if (data.is_number_integer()) {
            ctx->ProducePrimitive(data.get<long>());
        }
        if (data.is_number_float()) {
            ctx->ProducePrimitive(data.get<double>());
        }
        if (data.is_boolean()) {
            ctx->ProducePrimitive(data.get<bool>());
        }
        if (data.is_object()) {
            JSONMappingContext mapping_data;
            for(const auto& [k,v]: data.items()) {
                mapping_data[k] =CreateJSONContext(v);
            }
            ctx->ProduceMappingData(mapping_data);
        }
        return ctx;
    }

    static JSONContextPtr CloneJSONContext(const JSONContextPtr& ctx) {
        JSONContextPolicy policy {ctx->GetValue()};
        return  std::make_shared<IContext<JSONContextPolicy>>(policy);
    }

    static JSONContextPtr CreateJSONContextWithString(const std::string& json_string = "{}") {
        nlohmann::json json_obj = json_string.empty() ?
                nlohmann::json::parse("{}") :
                nlohmann::json::parse(json_string);
        return CreateJSONContext(json_obj);
    }

    static std::vector<JSONContextPtr> CreateBatchJSONContextWithString(const std::string& json_string = "[]") {
        nlohmann::json json_obj = json_string.empty() ?
                                  nlohmann::json::parse("[]") :
                                  nlohmann::json::parse(json_string);
        if (!json_obj.is_array()) {
            json_obj = nlohmann::json { json_obj };
        }
        std::vector<JSONContextPtr> result;
        result.reserve(json_obj.size());
        for(auto& item: json_obj) {
            result.push_back(CreateJSONContext(item));
        }
        return result;
    }

    static JSONObject SanitizeJSONContext(const JSONContextPtr& context) { // NOLINT(*-no-recursion)
        if (context->IsMessage()) {
            return nlohmann::json::parse(context->GetValue().at(MESSAGE_WRAPPER_DATA_KEY).template get<std::string>());
        }
        if(context->IsMappingObject()) {
            JSONObject sanitized;
            auto mapping_data = context->RequireMappingData();
            for(const auto &[k,v]: mapping_data) {
                sanitized[k] = SanitizeJSONContext(v);
            }
            return sanitized;
        }
        // primitive
        return context->GetValue();
    }

    static std::string DumpJSONContext(const JSONContextPtr& context) {
        return SanitizeJSONContext(context).dump();
    }

}


#endif //INSTINCT_JSONCONTEXTPOLICY_HPP
