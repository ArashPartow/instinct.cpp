//
// Created by RobinQu on 2024/4/25.
//

#ifndef MESSAGESERVICEIMPL_HPP
#define MESSAGESERVICEIMPL_HPP

#include <instinct/assistant/v2/service/message_service.hpp>
#include <instinct/assistant/v2/tool/entity_sql_utils.hpp>
#include <instinct/database/data_template.hpp>


namespace INSTINCT_ASSISTANT_NS::v2 {

    class MessageServiceImpl final: public IMessageService {
        data::DataTemplatePtr<MessageObject, std::string> data_mapper_;
    public:
        explicit MessageServiceImpl(const DataTemplatePtr<MessageObject, std::string> &data_mapper) : data_mapper_(
                data_mapper) {}

        ListMessageResponse ListMessages(const ListMessagesRequest &list_request) override {
            trace_span span {"ListMessages"};
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(list_request, context);

            // plus one for remaining check
            const auto limit = list_request.limit() <= 0 ? DEFAULT_LIST_LIMIT + 1: list_request.limit() + 1;
            context["limit"] = limit;
            if (list_request.order() == unknown_list_request_order) {
                context["order"] = "desc";
            }

            const auto messages_list = EntitySQLUtils::SelectManyMessages(data_mapper_, context);

            const auto n = messages_list.size();
            ListMessageResponse list_message_response;
            list_message_response.set_object("list");
            if (n>=limit) { // there is more, let's return all but last one
                list_message_response.set_has_more(true);
                list_message_response.set_first_id(messages_list.front().id());
                list_message_response.set_last_id(messages_list.at(n-2).id());
                list_message_response.mutable_data()->Add(messages_list.begin(), messages_list.end()-1);
            } else { // all results should be returned
                list_message_response.set_has_more(false);
                if (n > 0) {
                    list_message_response.set_first_id(messages_list.front().id());
                    list_message_response.set_last_id(messages_list.back().id());
                    list_message_response.mutable_data()->Add(messages_list.begin(), messages_list.end());
                }
                // do nothing if n == 0
            }
            return list_message_response;
        }

        std::optional<MessageObject> CreateMessage(const CreateMessageRequest &create_request) override {
            trace_span span {"CreateMessage"};
            const auto& thread_id = create_request.thread_id();
            assert_not_blank(thread_id, "should provide thread_id");

            MessageObject message_object;
            message_object.set_thread_id(create_request.thread_id());
            message_object.set_completed_at(ChronoUtils::GetCurrentEpochMicroSeconds());
            auto* msg = message_object.add_content();
            msg->mutable_text()->set_value(create_request.content());
            msg->set_type(MessageObject_MessageContentType_text);
            message_object.set_role(create_request.role());
            message_object.set_run_id(create_request.run_id());
            message_object.set_status(MessageObject_MessageStatus_completed);

            return CreateRawMessage(message_object);
        }

        std::optional<MessageObject> CreateRawMessage(const MessageObject &message_object) override {
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(message_object, context);

            // create msg id
            auto id = details::generate_next_object_id("msg");
            context["id"] = id;

            // insert
            EntitySQLUtils::InsertOneMessages(data_mapper_, context);

            // return
            GetMessageRequest get_message_request;
            get_message_request.set_message_id(id);
            get_message_request.set_thread_id(message_object.thread_id());
            return RetrieveMessage(get_message_request);
        }

        std::optional<MessageObject> RetrieveMessage(const GetMessageRequest &get_request) override {
            trace_span span {"RetrieveMessage"};
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(get_request, context);
            return EntitySQLUtils::SelectOneMessages(data_mapper_, context);
        }

        std::optional<MessageObject> ModifyMessage(const ModifyMessageRequest &modify_request) override {
            trace_span span {"ModifyMessage"};
            assert_not_blank(modify_request.message_id(), "should provide message id");
            assert_not_blank(modify_request.thread_id(), "should provide thread id");
            SQLContext context;
            ProtobufUtils::ConvertMessageToJsonObject(modify_request, context);
            EntitySQLUtils::UpdateMessage(data_mapper_, context);
            GetMessageRequest get_message_request;
            get_message_request.set_message_id(modify_request.message_id());
            get_message_request.set_thread_id(modify_request.thread_id());
            return RetrieveMessage(get_message_request);
        }
    };
}


#endif //MESSAGESERVICEIMPL_HPP
