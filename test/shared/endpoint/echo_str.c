#include "echo_str.h"

#include "id.h"

ReturnCode TestEndpointEchoStr(
        TestEndpointEchoStrRequest* request,
        TestEndpointEchoStrResponse* response)
{
    response->value = request->value;
    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(TEST_ENDPOINT_ID_ECHO_STR, TestEndpointEchoStr)
