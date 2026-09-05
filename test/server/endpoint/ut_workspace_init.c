#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>


TEST(WorkspaceInitTest_CallEndpoint_Success)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    Endpoint endpoint = Endpoint_WorkspaceInit_Create();
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, endpoint);

    ASSERT_EQ(endpointRegistry.__endpointCount, 1);
    ASSERT_EQ(endpointRegistry.__endpoints[ENDPOINT_ID_WORKSPACE_INIT].id, ENDPOINT_ID_WORKSPACE_INIT);

    Endpoint_WorkspaceInit_Request request = {
        .workspacePath = Str_FromCStr(projectDir),
        .workspaceName = Str_FromCStr("test_project"),
    };

    DataStream requestData = DataStream_New();
    Endpoint_WorkspaceInit_RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_INIT,
            &arena,
            &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_OK);
    EXPECT_EQ(responseHeader.msgLen, sizeof(i32) + sizeof(u64));

    Endpoint_WorkspaceInit_Response response;
    Endpoint_WorkspaceInit_ResponseDeserializer(&response, &responseData, &arena);
    EXPECT_EQ(response.workspaceId, 0);

    Str wetmanDir = FS_PathJoin(
            Str_FromCStr(projectDir),
            Str_FromCStr(".wetman"),
            &arena);
    EXPECT(FS_CheckExists(wetmanDir));

    Str workspaceConfigPath = FS_PathJoin(
            wetmanDir,
            Str_FromCStr("workspace.wmwscfg"),
            &arena);
    EXPECT(FS_CheckExists(workspaceConfigPath));

    PersistenceStatus readStatus;
    WorkspaceConfig readConfig = WorkspaceConfig_Read(
            workspaceConfigPath, &arena, &readStatus);
    EXPECT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(readConfig.workspaceId, 0);
    EXPECT_EQ(readConfig.workspaceName.len, 12);
    EXPECT(Str_EqStr(readConfig.workspaceName, Str_FromCStr("test_project")));

    Str serverWorkspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0"),
            &arena);
    EXPECT(FS_CheckExists(serverWorkspaceDir));

    Str serverTasksDir = FS_PathJoin(
            serverWorkspaceDir,
            Str_FromCStr("tasks"),
            &arena);
    EXPECT(FS_CheckExists(serverTasksDir));

    Str linkedConfigPath = FS_PathJoin(
            serverWorkspaceDir,
            Str_FromCStr("workspace.wmwscfg"),
            &arena);
    EXPECT(FS_CheckExists(linkedConfigPath));

    // Deallocates arena
    ServerContext_Destroy();
}

