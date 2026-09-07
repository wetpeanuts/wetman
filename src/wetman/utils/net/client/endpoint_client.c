#include <wetman/utils/net/client/endpoint_client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>
#include <wetman/utils/type.h>


typedef struct {
    EndpointRegistry* endpointRegistry;
} __EndpointClientContext;

Message __EndpointClient_RequestHandler(
        Client*  client,
        Message* requestMessage,
        Arena*   arena)
{
    __EndpointClientContext* context = (__EndpointClientContext*)client->__context;

    RequestHeader requestHeader = RequestHeader_Deserialize(&requestMessage->bodyStream);
    if (requestMessage->bodyStream.lastResult != DATA_STREAM_RESULT_SUCCESS) {
        Message responseMessage = Message_New();
        ResponseHeader responseHeader = {
            .returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST,
            .msgLen     = 0,
        };
        ResponseHeader_Serialize(&responseHeader, &responseMessage.bodyStream, arena);
        return responseMessage;
    }

    return EndpointRegistry_CallEndpoint(
            context->endpointRegistry,
            requestHeader.endpointId,
            arena,
            requestMessage);
}

void __EndpointClient_DisconnectHandler(Client* client)
{
    Arena_Free(&client->__arena);
}

Client EndpointClient_Connect(EndpointRegistry* endpointRegistry)
{
    Arena arena = Arena_WithPageCapacity(sizeof(__EndpointClientContext));
    __EndpointClientContext* context = (__EndpointClientContext*)Arena_Alloc(
            &arena, sizeof(__EndpointClientContext));

    context->endpointRegistry = endpointRegistry;

    Client client = {
        .__arena          = arena,
        .__requestHandler = __EndpointClient_RequestHandler,
        .__context        = (void*)context,
        .disconnect       = __EndpointClient_DisconnectHandler,
    };

    return client;
}
