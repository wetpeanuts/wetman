#ifndef TEST_SHARED_ENDPOINT_ECHO_FD_H
#define TEST_SHARED_ENDPOINT_ECHO_FD_H

#include <wetman/utils/macro.h>
#include <wetman/utils/type.h>
#include <wetman/utils/net/fd.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/message.h>

typedef struct {
    FileDescriptor fd;
} TestEndpointEchoFd_Request;

typedef struct {
    FileDescriptor fd;
} TestEndpointEchoFd_Response;

static inline MAYBE_UNUSED void TestEndpointEchoFd_RequestSerializer(
        TestEndpointEchoFd_Request* req,
        Message*                    message,
        Arena*                      arena)
{
    Message_WriteFd(message, req->fd, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoFd_RequestDeserializer(
        TestEndpointEchoFd_Request* req,
        Message*                    message,
        Arena*                      arena)
{
    req->fd = Message_ReadFd(message);
    (void)arena;
}

static inline MAYBE_UNUSED void TestEndpointEchoFd_ResponseSerializer(
        TestEndpointEchoFd_Response* resp,
        Message*                     message,
        Arena*                       arena)
{
    Message_WriteFd(message, resp->fd, arena);
}

static inline MAYBE_UNUSED void TestEndpointEchoFd_ResponseDeserializer(
        TestEndpointEchoFd_Response* resp,
        Message*                     message,
        Arena*                       arena)
{
    resp->fd = Message_ReadFd(message);
    (void)arena;
}

ENDPOINT_DECLARE_SERVER(TestEndpointEchoFd)

#endif // TEST_SHARED_ENDPOINT_ECHO_FD_H
