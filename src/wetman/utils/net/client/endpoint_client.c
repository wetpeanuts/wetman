#include <wetman/utils/net/client/endpoint_client.h>

#include <wetman/utils/net/message.h>
#include <wetman/utils/net/return_code.h>
#include <wetman/utils/type.h>


typedef struct {
    EndpointRegistry* endpointRegistry;
} __EndpointClientContext;

DataStream __EndpointClient_RequestHandler(
        Client*     client,
        DataStream* requestData,
        Arena*      arena)
{
    __EndpointClientContext* context = (__EndpointClientContext*)client->__context;

    RequestHeader requestHeader = RequestHeader_Deserialize(requestData);
    if (requestData->lastResult != DATA_STREAM_RESULT_SUCCESS) {
        DataStream responseData = DataStream_New();
        ResponseHeader responseHeader = {
            .returnCode = RETURN_CODE_FAILED_TO_PARSE_REQUEST,
            .msgLen     = 0,
        };
        ResponseHeader_Serialize(&responseHeader, &responseData, arena);
        return responseData;
    }

    return EndpointRegistry_CallEndpoint(
            context->endpointRegistry,
            requestHeader.endpointId,
            arena,
            requestData);
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
