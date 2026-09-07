#ifndef WETMAN_SHARED_ENDPOINT_WORKSPACE_LIST_H
#define WETMAN_SHARED_ENDPOINT_WORKSPACE_LIST_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/data_struct/slice_str.h>
#include <wetman/utils/data_struct/slice_u64.h>
#include <wetman/utils/net/message.h>


typedef struct {
    i32 __dummy;
} Endpoint_WorkspaceList_Request;

typedef struct {
    SliceU64 workspaceIds;
    SliceStr workspaceNames;
    SliceStr workspacePaths;
} Endpoint_WorkspaceList_Response;

static inline MAYBE_UNUSED void Endpoint_WorkspaceList_RequestSerializer(
        Endpoint_WorkspaceList_Request* req,
        Message*                        message,
        Arena*                          arena)
{
    (void)req;
    (void)message;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceList_RequestDeserializer(
        Endpoint_WorkspaceList_Request* req,
        Message*                        message,
        Arena*                          arena)
{
    (void)req;
    (void)message;
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceList_ResponseSerializer(
        Endpoint_WorkspaceList_Response* resp,
        Message*                         message,
        Arena*                           arena)
{
    DataStream_PushSliceU64(&message->bodyStream, resp->workspaceIds, arena);
    DataStream_PushSliceStr(&message->bodyStream, resp->workspaceNames, arena);
    DataStream_PushSliceStr(&message->bodyStream, resp->workspacePaths, arena);
}

static inline MAYBE_UNUSED void Endpoint_WorkspaceList_ResponseDeserializer(
        Endpoint_WorkspaceList_Response* resp,
        Message*                         message,
        Arena*                           arena)
{
    resp->workspaceIds   = DataStream_PopSliceU64(&message->bodyStream);
    resp->workspaceNames = DataStream_PopSliceStr(&message->bodyStream, arena);
    resp->workspacePaths = DataStream_PopSliceStr(&message->bodyStream, arena);
}

#endif // WETMAN_SHARED_ENDPOINT_WORKSPACE_LIST_H
