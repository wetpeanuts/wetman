#ifndef WETMAN_SHARED_ENDPOINT_TASK_LIST_H
#define WETMAN_SHARED_ENDPOINT_TASK_LIST_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/data_struct/slice_str.h>
#include <wetman/utils/data_struct/slice_u64.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
} Endpoint_TaskList_Request;

typedef struct {
    SliceU64 taskIds;
    SliceStr taskNames;
    SliceStr taskPaths;
} Endpoint_TaskList_Response;

static inline MAYBE_UNUSED void Endpoint_TaskList_RequestSerializer(
        Endpoint_TaskList_Request* req,
        Message*                   message,
        Arena*                     arena)
{
    DataStream_PushU64(&message->body, (u64)req->workspaceId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskList_RequestDeserializer(
        Endpoint_TaskList_Request* req,
        Message*                   message,
        Arena*                     arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_TaskList_ResponseSerializer(
        Endpoint_TaskList_Response* resp,
        Message*                    message,
        Arena*                      arena)
{
    DataStream_PushSliceU64(&message->body, resp->taskIds, arena);
    DataStream_PushSliceStr(&message->body, resp->taskNames, arena);
    DataStream_PushSliceStr(&message->body, resp->taskPaths, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskList_ResponseDeserializer(
        Endpoint_TaskList_Response* resp,
        Message*                    message,
        Arena*                      arena)
{
    resp->taskIds   = DataStream_PopSliceU64(&message->body);
    resp->taskNames = DataStream_PopSliceStr(&message->body, arena);
    resp->taskPaths = DataStream_PopSliceStr(&message->body, arena);
}

#endif // WETMAN_SHARED_ENDPOINT_TASK_LIST_H
