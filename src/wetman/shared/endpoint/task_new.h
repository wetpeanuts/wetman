#ifndef WETMAN_SHARED_ENDPOINT_TASK_NEW_H
#define WETMAN_SHARED_ENDPOINT_TASK_NEW_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
    Str   taskName;
} Endpoint_TaskNew_Request;

typedef struct {
    usize taskId;
} Endpoint_TaskNew_Response;

static inline MAYBE_UNUSED void Endpoint_TaskNew_RequestSerializer(
        Endpoint_TaskNew_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    DataStream_PushU64(&message->bodyStream, (u64)req->workspaceId, arena);
    DataStream_PushStr(&message->bodyStream, req->taskName, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskNew_RequestDeserializer(
        Endpoint_TaskNew_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->bodyStream);
    req->taskName    = DataStream_PopStr(&message->bodyStream);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_TaskNew_ResponseSerializer(
        Endpoint_TaskNew_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    DataStream_PushU64(&message->bodyStream, (u64)resp->taskId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskNew_ResponseDeserializer(
        Endpoint_TaskNew_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    resp->taskId = (usize)DataStream_PopU64(&message->bodyStream);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_TASK_NEW_H
