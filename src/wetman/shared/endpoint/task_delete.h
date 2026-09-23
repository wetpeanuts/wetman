#ifndef WETMAN_SHARED_ENDPOINT_TASK_DELETE_H
#define WETMAN_SHARED_ENDPOINT_TASK_DELETE_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
    usize taskId;
} Endpoint_TaskDelete_Request;

typedef struct {
    usize taskId;
} Endpoint_TaskDelete_Response;

static inline MAYBE_UNUSED void Endpoint_TaskDelete_RequestSerializer(
        Endpoint_TaskDelete_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    DataStream_PushU64(&message->body, (u64)req->workspaceId, arena);
    DataStream_PushU64(&message->body, (u64)req->taskId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskDelete_RequestDeserializer(
        Endpoint_TaskDelete_Request* req,
        Message*                     message,
        Arena*                       arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->body);
    req->taskId      = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_TaskDelete_ResponseSerializer(
        Endpoint_TaskDelete_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    DataStream_PushU64(&message->body, (u64)resp->taskId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskDelete_ResponseDeserializer(
        Endpoint_TaskDelete_Response* resp,
        Message*                      message,
        Arena*                        arena)
{
    resp->taskId = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_TASK_DELETE_H
