#ifndef WETMAN_SHARED_ENDPOINT_WORKSPACE_DELETE_H
#define WETMAN_SHARED_ENDPOINT_WORKSPACE_DELETE_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
} Endpoint_WorkspaceDelete_Request;

typedef struct {
    usize workspaceId;
} Endpoint_WorkspaceDelete_Response;

static inline MAYBE_UNUSED void Endpoint_WorkspaceDelete_RequestSerializer(
        Endpoint_WorkspaceDelete_Request* req,
        Message*                          message,
        Arena*                            arena)
{
    DataStream_PushU64(&message->bodyStream, (u64)req->workspaceId, arena);
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceDelete_RequestDeserializer(
        Endpoint_WorkspaceDelete_Request* req,
        Message*                          message,
        Arena*                            arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->bodyStream);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceDelete_ResponseSerializer(
        Endpoint_WorkspaceDelete_Response* resp,
        Message*                           message,
        Arena*                             arena)
{
    DataStream_PushU64(&message->bodyStream, (u64)resp->workspaceId, arena);
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceDelete_ResponseDeserializer(
        Endpoint_WorkspaceDelete_Response* resp,
        Message*                           message,
        Arena*                             arena)
{
    resp->workspaceId = (usize)DataStream_PopU64(&message->bodyStream);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_WORKSPACE_DELETE_H
