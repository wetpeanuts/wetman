#include <wetman/server/endpoint/task_get.h>
#include <wetman/server/context.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>

#include "utils.h"


static ReturnCode GetTask(
        EndpointRegistry*           endpointRegistry,
        Arena*                      arena,
        usize                       workspaceId,
        usize                       taskId,
        Endpoint_TaskGet_Response*  response)
{
    Endpoint_TaskGet_Request request = {
        .workspaceId = workspaceId,
        .taskId      = taskId,
    };

    Message requestMessage = Message_New();
    Endpoint_TaskGet_RequestSerializer(&request, &requestMessage, arena);

    Message responseMessage = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_TASK_GET,
            arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    if (responseHeader.returnCode == RETURN_CODE_OK) {
        Endpoint_TaskGet_ResponseDeserializer(response, &responseMessage, arena);
    }

    return (ReturnCode)responseHeader.returnCode;
}


TEST(TaskGetTest_CallEndpoint_Success)
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
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskGet_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskNew_Response taskNewResponse = {0};
    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "test_task", &taskNewResponse), RETURN_CODE_OK);
    EXPECT_EQ(taskNewResponse.taskId, 0);

    Endpoint_TaskGet_Response response = {0};
    EXPECT_EQ(GetTask(&endpointRegistry, &arena, 0, 0, &response), RETURN_CODE_OK);

    EXPECT_EQ(response.taskId, 0);
    EXPECT(Str_EqCStr(response.taskName, "test_task"));

    Str taskDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0/tasks/0"),
            &arena);
    Str expectedTaskPath = FS_PathJoin(taskDir, Str_FromCStr("task.sh"), &arena);

    EXPECT(Str_EqStr(response.taskPath, expectedTaskPath));
    EXPECT(FS_CheckExists(response.taskPath));

    ServerContext_Destroy();
}

TEST(TaskGetTest_CallEndpoint_TaskNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskGet_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskGet_Response response = {0};
    EXPECT_EQ(GetTask(&endpointRegistry, &arena, 0, 999, &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}

TEST(TaskGetTest_CallEndpoint_WorkspaceNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskGet_Create());

    Endpoint_TaskGet_Response response = {0};
    EXPECT_EQ(GetTask(&endpointRegistry, &arena, 999, 0, &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}
