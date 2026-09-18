#include <wetman/server/endpoint/task_list.h>
#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/test/macro.h>

#include "utils.h"


static ReturnCode ListTasks(
        EndpointRegistry*           endpointRegistry,
        Arena*                      arena,
        usize                       workspaceId,
        Endpoint_TaskList_Response* response)
{
    Endpoint_TaskList_Request request = {
        .workspaceId = workspaceId,
    };

    Message requestMessage = Message_New();
    Endpoint_TaskList_RequestSerializer(&request, &requestMessage, arena);

    Message responseMessage = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_TASK_LIST,
            arena,
            &requestMessage);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseMessage.header);

    if (responseHeader.returnCode == RETURN_CODE_OK) {
        Endpoint_TaskList_ResponseDeserializer(response, &responseMessage, arena);
    }

    return (ReturnCode)responseHeader.returnCode;
}


TEST(TaskListTest_CallEndpoint_Success)
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
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskList_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskNew_Response taskNewResponse = {0};
    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "test_task_one", &taskNewResponse), RETURN_CODE_OK);
    EXPECT_EQ(taskNewResponse.taskId, 0);
    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "test_task_two", &taskNewResponse), RETURN_CODE_OK);
    EXPECT_EQ(taskNewResponse.taskId, 1);

    Endpoint_TaskList_Response response = {0};
    EXPECT_EQ(ListTasks(&endpointRegistry, &arena, 0, &response), RETURN_CODE_OK);

    EXPECT_EQ(response.taskIds.len, 2);
    EXPECT_EQ(response.taskNames.len, response.taskIds.len);
    EXPECT_EQ(response.taskPaths.len, response.taskIds.len);

    EXPECT_EQ(response.taskIds.data[0], 0);
    EXPECT_EQ(response.taskIds.data[1], 1);

    EXPECT(Str_EqCStr(response.taskNames.data[0], "test_task_one"));
    EXPECT(Str_EqCStr(response.taskNames.data[1], "test_task_two"));

    Str taskDir0 = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0/tasks/0"),
            &arena);
    Str expectedTaskPath0 = FS_PathJoin(taskDir0, Str_FromCStr("task.sh"), &arena);
    EXPECT(Str_EqStr(response.taskPaths.data[0], expectedTaskPath0));

    Str taskDir1 = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0/tasks/1"),
            &arena);
    Str expectedTaskPath1 = FS_PathJoin(taskDir1, Str_FromCStr("task.sh"), &arena);
    EXPECT(Str_EqStr(response.taskPaths.data[1], expectedTaskPath1));

    EXPECT(FS_CheckExists(expectedTaskPath0));
    EXPECT(FS_CheckExists(expectedTaskPath1));

    ServerContext_Destroy();
}

TEST(TaskListTest_CallEndpoint_Empty)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskList_Create());

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskList_Response response = {0};
    EXPECT_EQ(ListTasks(&endpointRegistry, &arena, 0, &response), RETURN_CODE_OK);

    EXPECT_EQ(response.taskIds.len, 0);
    EXPECT_EQ(response.taskNames.len, 0);
    EXPECT_EQ(response.taskPaths.len, 0);

    ServerContext_Destroy();
}

TEST(TaskListTest_CallEndpoint_WorkspaceNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskList_Create());

    Endpoint_TaskList_Response response = {0};
    EXPECT_EQ(ListTasks(&endpointRegistry, &arena, 999, &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}
