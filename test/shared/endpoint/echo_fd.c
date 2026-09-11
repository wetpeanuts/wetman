#include "echo_fd.h"

#include "id.h"

ReturnCode TestEndpointEchoFd(
        TestEndpointEchoFd_Request*  request,
        TestEndpointEchoFd_Response* response,
        Arena*                       arena)
{
    (void)arena;
    response->fd = request->fd;
    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(TEST_ENDPOINT_ID_ECHO_FD, TestEndpointEchoFd)
