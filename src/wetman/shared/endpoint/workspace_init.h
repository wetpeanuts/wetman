#ifndef WETMAN_SHARED_ENDPOINT_WORKSPACE_INIT_H
#define WETMAN_SHARED_ENDPOINT_WORKSPACE_INIT_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>


typedef struct {
    Str workspacePath;
    Str workspaceName;
} Endpoint_WorkspaceInit_Request;

typedef struct {
    usize workspaceId;
} Endpoint_WorkspaceInit_Response;

static inline MAYBE_UNUSED void Endpoint_WorkspaceInit_RequestSerializer(
        Endpoint_WorkspaceInit_Request* req,
        Message*                        message,
        Arena*                          arena)
{
    DataStream_PushStr(&message->body, req->workspacePath, arena);
    DataStream_PushStr(&message->body, req->workspaceName, arena);
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceInit_RequestDeserializer(
        Endpoint_WorkspaceInit_Request* req,
        Message*                        message,
        Arena*                          arena)
{
    req->workspacePath = DataStream_PopStr(&message->body);
    req->workspaceName = DataStream_PopStr(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceInit_ResponseSerializer(
        Endpoint_WorkspaceInit_Response* resp,
        Message*                         message,
        Arena*                           arena)
{
    DataStream_PushU64(&message->body, (u64)resp->workspaceId, arena);
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceInit_ResponseDeserializer(
        Endpoint_WorkspaceInit_Response* resp,
        Message*                         message,
        Arena*                           arena)
{
    resp->workspaceId = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_WORKSPACE_INIT_H
