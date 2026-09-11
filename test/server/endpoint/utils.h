#ifndef WETMAN_TEST_SERVER_ENDPOINT_UTILS_H
#define WETMAN_TEST_SERVER_ENDPOINT_UTILS_H

#include <wetman/server/endpoint/task_new.h>
#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>


static inline MAYBE_UNUSED ReturnCode InitTestWorkspace(
        EndpointRegistry* endpointRegistry,
        Arena*            arena,
        const char*       projectDir,
        const char*       workspaceName)
{
    Endpoint_WorkspaceInit_Request request = {
        .workspacePath = Str_FromCStr(projectDir),
        .workspaceName = Str_FromCStr(workspaceName),
    };

    Message requestMessage = Message_New();
    Endpoint_WorkspaceInit_RequestSerializer(&request, &requestMessage, arena);

    Message responseMessage = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_WORKSPACE_INIT,
            arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    return (ReturnCode)responseHeader.returnCode;
}

static inline MAYBE_UNUSED ReturnCode CreateTask(
        EndpointRegistry*           endpointRegistry,
        Arena*                      arena,
        usize                       workspaceId,
        const char*                 taskName,
        Endpoint_TaskNew_Response*  response)
{
    Endpoint_TaskNew_Request request = {
        .workspaceId = workspaceId,
        .taskName    = Str_FromCStr(taskName),
    };

    Message requestMessage = Message_New();
    Endpoint_TaskNew_RequestSerializer(&request, &requestMessage, arena);

    Message responseMessage = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_TASK_NEW,
            arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    if (responseHeader.returnCode == RETURN_CODE_OK) {
        Endpoint_TaskNew_ResponseDeserializer(response, &responseMessage, arena);
    }

    return (ReturnCode)responseHeader.returnCode;
}

#endif // WETMAN_TEST_SERVER_ENDPOINT_UTILS_H
