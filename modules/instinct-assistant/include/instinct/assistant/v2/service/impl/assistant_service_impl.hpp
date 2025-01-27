//
// Created by RobinQu on 2024/4/22.
//

#ifndef ASSISTANTSERVICEIMPL_HPP
#define ASSISTANTSERVICEIMPL_HPP

#include <instinct/assistant_global.hpp>
#include <instinct/assistant/v2/service/assistant_service.hpp>
#include <instinct/assistant/v2/tool/entity_sql_utils.hpp>
#include <instinct/database/data_template.hpp>

namespace INSTINCT_ASSISTANT_NS::v2 {
    using namespace INSTINCT_DATA_NS;

    class AssistantServiceImpl final: public IAssistantService {
        DataTemplatePtr<AssistantObject, std::string> data_mapper_;
    public:
        explicit AssistantServiceImpl(const DataTemplatePtr<AssistantObject, std::string> &data_mapper)
            : data_mapper_(data_mapper) {
        }

        ListAssistantsResponse ListAssistants(const ListAssistantsRequest &list_request) override {
            trace_span span {"ListAssistants"};
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(list_request, context, {.keep_default_values = true});
            // limit + 1 to check if there is more records that match the conditions
            auto limit = list_request.limit() <= 0 ? DEFAULT_LIST_LIMIT + 1 : list_request.limit() + 1;
            context["limit"] = limit;
            if (list_request.order() == unknown_list_request_order) {
                context["order"] = "desc";
            }
            auto assistants = EntitySQLUtils::GetManyAssistant(data_mapper_, context);

            ListAssistantsResponse response;
            if (const auto n = assistants.size(); n>=limit) {
                response.set_first_id(assistants.front().id());
                response.set_last_id(assistants[n-2].id());
                response.mutable_data()->Add(assistants.begin(), assistants.end()-1);
                response.set_has_more(true);
            } else {
                response.set_has_more(false);
                if (n>0) {
                    response.mutable_data()->Add(assistants.begin(), assistants.end());
                    response.set_first_id(assistants.front().id());
                    response.set_last_id(assistants.back().id());
                }
                // do nothing if n == 0
            }
            response.set_object("list");
            return response;
        }

        std::optional<AssistantObject> CreateAssistant(const AssistantObject &create_request) override {
            trace_span span {"CreateAssistant"};
            assert_not_blank(create_request.model(), "should provide model name");
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(create_request, context, {.keep_default_values = true});

            if (!create_request.has_top_p()) {
                context["top_p"] = 1;
            }
            if (!create_request.has_temperature()) {
                context["temperature"] = 1;
            }
            if (StringUtils::IsBlankString(create_request.response_format())) {
                context["response_format"] = "auto";
            }

            auto id = details::generate_next_object_id("asst");
            context["id"] = id;
            EntitySQLUtils::InsertOneAssistant(data_mapper_, context);

            GetAssistantRequest get_assistant_request;
            get_assistant_request.set_assistant_id(id);
            return this->RetrieveAssistant(get_assistant_request);
        }

        std::optional<AssistantObject> RetrieveAssistant(const GetAssistantRequest &get_request) override {
            trace_span span {"RetrieveAssistant"};
            return EntitySQLUtils::GetOneAssistant(data_mapper_, {{"id", get_request.assistant_id()}});
        }

        DeleteAssistantResponse DeleteAssistant(const DeleteAssistantRequest &delete_request) override {
            trace_span span {"DeleteAssistant"};
            const auto count = EntitySQLUtils::DeleteAssistant(data_mapper_, {{"id", delete_request.assistant_id()}});

            DeleteAssistantResponse response;
            response.set_id(delete_request.assistant_id());
            response.set_deleted(count == 1);
            response.set_object("assistant.deleted");
            return response;
        }

        std::optional<AssistantObject> ModifyAssistant(const ModifyAssistantRequest &modify_assistant_request) override {
            trace_span span {"ModifyAssistant"};
            assert_not_blank(modify_assistant_request.assistant_id(), "assistant id should be given");
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(modify_assistant_request, context);
            const auto count = EntitySQLUtils::UpdateAssistant(data_mapper_, context);
            assert_positive(count, "should have assistant update. assistant_id=" + modify_assistant_request.assistant_id());
            GetAssistantRequest get_assistant_request;
            get_assistant_request.set_assistant_id(modify_assistant_request.assistant_id());
            return this->RetrieveAssistant(get_assistant_request);
        }

    };
}


#endif //ASSISTANTSERVICEIMPL_HPP
