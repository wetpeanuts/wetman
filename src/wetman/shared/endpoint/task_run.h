#ifndef WETMAN_SHARED_ENDPOINT_TASK_RUN_H
#define WETMAN_SHARED_ENDPOINT_TASK_RUN_H

#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/endpoint.h>
#include <wetman/utils/net/fd.h>
#include <wetman/utils/net/message.h>


typedef struct {
    usize workspaceId;
    usize taskId;
} Endpoint_TaskRun_Request;

// The server forks the task as a child process and redirects its stdout/stderr
// into a pipe. The read end of that pipe is handed back to the client through
// SCM_RIGHTS; the client reads it until EOF (when the task process exits).
typedef struct {
    FileDescriptor stdoutFd;
} Endpoint_TaskRun_Response;

static inline MAYBE_UNUSED void Endpoint_TaskRun_RequestSerializer(
        Endpoint_TaskRun_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    DataStream_PushU64(&message->body, (u64)req->workspaceId, arena);
    DataStream_PushU64(&message->body, (u64)req->taskId, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskRun_RequestDeserializer(
        Endpoint_TaskRun_Request* req,
        Message*                  message,
        Arena*                    arena)
{
    req->workspaceId = (usize)DataStream_PopU64(&message->body);
    req->taskId      = (usize)DataStream_PopU64(&message->body);
    (void)arena;
}

static inline MAYBE_UNUSED void Endpoint_TaskRun_ResponseSerializer(
        Endpoint_TaskRun_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    FdStream_Push(&message->fileDescriptors, resp->stdoutFd, arena);
}

static inline MAYBE_UNUSED void Endpoint_TaskRun_ResponseDeserializer(
        Endpoint_TaskRun_Response* resp,
        Message*                   message,
        Arena*                     arena)
{
    resp->stdoutFd = FdStream_Pop(&message->fileDescriptors);
    (void)arena;
}

#endif // WETMAN_SHARED_ENDPOINT_TASK_RUN_H
