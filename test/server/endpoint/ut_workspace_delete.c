#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/endpoint/workspace_delete.h>
#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>


TEST(WorkspaceDeleteTest_CallEndpoint_Success)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char projectDir[256];
    CREATE_TMP_DIR(projectDir);

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceDelete_Create());

    ASSERT_EQ(endpointRegistry.__endpointCount, 2);

    // Init workspace first
    Endpoint_WorkspaceInit_Request initRequest = {
        .workspacePath = Str_FromCStr(projectDir),
        .workspaceName = Str_FromCStr("test_project"),
    };

    DataStream initRequestData = DataStream_New();
    Endpoint_WorkspaceInit_RequestSerializer(&initRequest, &initRequestData, &arena);

    DataStream initResponseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_INIT,
            &arena,
            &initRequestData);
    ResponseHeader initResponseHeader = ResponseHeader_Deserialize(&initResponseData);

    EXPECT_EQ(initResponseHeader.returnCode, RETURN_CODE_OK);

    Endpoint_WorkspaceInit_Response initResponse;
    Endpoint_WorkspaceInit_ResponseDeserializer(&initResponse, &initResponseData);
    EXPECT_EQ(initResponse.workspaceId, 0);

    // Verify workspace exists
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

    Str serverWorkspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0"),
            &arena);
    EXPECT(FS_CheckExists(serverWorkspaceDir));

    // Delete workspace
    Endpoint_WorkspaceDelete_Request deleteRequest = {
        .workspaceId = 0,
    };

    DataStream deleteRequestData = DataStream_New();
    Endpoint_WorkspaceDelete_RequestSerializer(&deleteRequest, &deleteRequestData, &arena);

    DataStream deleteResponseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_DELETE,
            &arena,
            &deleteRequestData);
    ResponseHeader deleteResponseHeader = ResponseHeader_Deserialize(&deleteResponseData);

    EXPECT_EQ(deleteResponseHeader.returnCode, RETURN_CODE_OK);

    Endpoint_WorkspaceDelete_Response deleteResponse;
    Endpoint_WorkspaceDelete_ResponseDeserializer(&deleteResponse, &deleteResponseData);
    EXPECT_EQ(deleteResponse.workspaceId, 0);

    // Verify workspace is deleted
    EXPECT(!FS_CheckExists(wetmanDir));
    EXPECT(!FS_CheckExists(serverWorkspaceDir));

    ServerContext_Destroy();
}

TEST(WorkspaceDeleteTest_CallEndpoint_WorkspaceNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceDelete_Create());

    Endpoint_WorkspaceDelete_Request request = {
        .workspaceId = 999,
    };

    DataStream requestData = DataStream_New();
    Endpoint_WorkspaceDelete_RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_DELETE,
            &arena,
            &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}
