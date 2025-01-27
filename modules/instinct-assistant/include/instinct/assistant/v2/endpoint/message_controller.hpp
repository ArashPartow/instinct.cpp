//
// Created by RobinQu on 2024/4/29.
//

#ifndef MESSAGECONTROLLER_HPP
#define MESSAGECONTROLLER_HPP

#include <instinct/assistant/v2/endpoint/base_controller.hpp>


namespace INSTINCT_ASSISTANT_NS::v2 {
    class MessageController final: public BaseController {
    public:
        explicit MessageController(const AssistantFacade &facade)
            : BaseController(facade) {
        }

        void Mount(HttpLibServer &server) override {
            server.GetRoute<ListMessagesRequest, ListMessageResponse>("/v1/threads/:thread_id/messages", [&](ListMessagesRequest& req, const HttpLibSession& session) {
                RequestUtils::LoadPaginationParameters(session.request, req);
                req.set_thread_id(session.request.path_params.at("thread_id"));
                if (session.request.path_params.contains("run_id")) {
                    req.set_run_id(session.request.path_params.at("run_id"));
                }
                const auto resp = facade_.message->ListMessages(req);
                session.Respond(resp);
            });

            server.PostRoute<CreateMessageRequest, MessageObject>("/v1/threads/:thread_id/messages", [&](CreateMessageRequest& req, const HttpLibSession& session) {
                req.set_thread_id(session.request.path_params.at("thread_id"));
                if (const auto resp = facade_.message->CreateMessage(req)) {
                    session.Respond(resp.value());
                } else {
                    session.Respond("Mesage object cannot be retireved after creation", 500);
                }
            });

            server.GetRoute<GetMessageRequest, MessageObject>("/v1/threads/:thread_id/messages/:message_id", [&](GetMessageRequest& req, const HttpLibSession& session) {
                req.set_thread_id(session.request.path_params.at("thread_id"));
                req.set_message_id(session.request.path_params.at("message_id"));
                if (const auto resp = facade_.message->RetrieveMessage(req)) {
                    session.Respond(resp.value());
                } else {
                    session.Respond(fmt::format("Message object cannot be found with thread_id({}), message_id({})", req.thread_id(), req.message_id()));
                }
            });

            server.PostRoute<ModifyMessageRequest, MessageObject>("/v1/threads/:thread_id/messages/:message_id", [&](ModifyMessageRequest& req, const HttpLibSession& session) {
                req.set_thread_id(session.request.path_params.at("thread_id"));
                req.set_message_id(session.request.path_params.at("message_id"));
                if (const auto resp = facade_.message->ModifyMessage(req)) {
                    session.Respond(resp.value());
                } else {
                    session.Respond("Message object cannot be retrieved after modification", 500);
                }
            });
        }
    };
}

#endif //MESSAGECONTROLLER_HPP
