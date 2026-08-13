#include "echo_i32.h"

#include "id.h"

ReturnCode TestEndpointEchoI32(
        TestEndpointEchoI32Request* request,
        TestEndpointEchoI32Response* response)
{
    response->value = request->value;
    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(TEST_ENDPOINT_ID_ECHO_I32, TestEndpointEchoI32)

