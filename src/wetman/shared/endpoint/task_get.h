#ifndef WETMAN_SHARED_ENDPOINT_TASK_GET_H
#define WETMAN_SHARED_ENDPOINT_TASK_GET_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
    usize taskId;
} Endpoint_TaskGet_Request;

typedef struct {
    usize taskId;
    Str   taskName;
    Str   taskPath;
} Endpoint_TaskGet_Response;

static inline MAYBE_UNUSED void Endpoint_TaskGet_RequestSerializer(
        Endpoint_TaskGet_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    DataStream_PushU64(&message->body, (u64)req->workspaceId, arena);
    DataStream_PushU64(&message->body, (u64)req->taskId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskGet_RequestDeserializer(
        Endpoint_TaskGet_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->body);
    req->taskId      = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_TaskGet_ResponseSerializer(
        Endpoint_TaskGet_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    DataStream_PushU64(&message->body, (u64)resp->taskId, arena);
    DataStream_PushStr(&message->body, resp->taskName, arena);
    DataStream_PushStr(&message->body, resp->taskPath, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskGet_ResponseDeserializer(
        Endpoint_TaskGet_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    resp->taskId   = (usize)DataStream_PopU64(&message->body);
    resp->taskName = DataStream_PopStr(&message->body);
    resp->taskPath = DataStream_PopStr(&message->body);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_TASK_GET_H
