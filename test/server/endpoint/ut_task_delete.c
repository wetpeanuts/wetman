#include <wetman/server/endpoint/task_delete.h>
#include <wetman/server/context.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>

#include "utils.h"


static ReturnCode DeleteTask(
        EndpointRegistry*     endpointRegistry,
        Arena*                arena,
        usize                 workspaceId,
        usize                 taskId,
        Endpoint_TaskDelete_Response* response)
{
    Endpoint_TaskDelete_Request request = {
        .workspaceId = workspaceId,
        .taskId      = taskId,
    };

    Message requestMessage = Message_New();
    Endpoint_TaskDelete_RequestSerializer(&request, &requestMessage, arena);

    Message responseMessage = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_TASK_DELETE,
            arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    if (responseHeader.returnCode == RETURN_CODE_OK) {
        Endpoint_TaskDelete_ResponseDeserializer(response, &responseMessage, arena);
    }

    return (ReturnCode)responseHeader.returnCode;
}


TEST(TaskDeleteTest_CallEndpoint_Success)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskNew_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskDelete_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskNew_Response taskNewResponse = {0};
    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "test_task", &taskNewResponse), RETURN_CODE_OK);
    EXPECT_EQ(taskNewResponse.taskId, 0);

    Str taskDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0/tasks/0"),
            &arena);
    EXPECT(FS_CheckExists(taskDir));

    Endpoint_TaskDelete_Response response = {0};
    EXPECT_EQ(DeleteTask(&endpointRegistry, &arena, 0, 0, &response), RETURN_CODE_OK);
    EXPECT_EQ(response.taskId, 0);

    EXPECT(!FS_CheckExists(taskDir));

    ServerContext_Destroy();
}

TEST(TaskDeleteTest_CallEndpoint_TaskNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskDelete_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskDelete_Response response = {0};
    EXPECT_EQ(DeleteTask(&endpointRegistry, &arena, 0, 999, &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}

TEST(TaskDeleteTest_CallEndpoint_WorkspaceNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskDelete_Create());

    Endpoint_TaskDelete_Response response = {0};
    EXPECT_EQ(DeleteTask(&endpointRegistry, &arena, 999, 0, &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}
