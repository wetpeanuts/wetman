#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/endpoint/workspace_list.h>
#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>


static ReturnCode InitWorkspace(
        EndpointRegistry*     endpointRegistry,
        Arena*                arena,
        const char*           projectDir,
        const char*           workspaceName)
{
    Endpoint_WorkspaceInit_Request request = {
        .workspacePath = Str_FromCStr(projectDir),
        .workspaceName = Str_FromCStr(workspaceName),
    };

    DataStream requestData = DataStream_New();
    Endpoint_WorkspaceInit_RequestSerializer(&request, &requestData, arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            endpointRegistry,
            ENDPOINT_ID_WORKSPACE_INIT,
            arena,
            &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    return (ReturnCode)responseHeader.returnCode;
}


TEST(WorkspaceListTest_CallEndpoint_Success)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceInit_Create());
    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceList_Create());

    ASSERT_EQ(endpointRegistry.__endpointCount, 2);

    char projectDir1[256];
    CREATE_TMP_DIR(projectDir1);
    char projectDir2[256];
    CREATE_TMP_DIR(projectDir2);

    EXPECT_EQ(InitWorkspace(&endpointRegistry, &arena, projectDir1, "workspace_one"), RETURN_CODE_OK);
    EXPECT_EQ(InitWorkspace(&endpointRegistry, &arena, projectDir2, "workspace_two"), RETURN_CODE_OK);

    Endpoint_WorkspaceList_Request request = {
        .__dummy = 0,
    };
    DataStream requestData = DataStream_New();
    Endpoint_WorkspaceList_RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_LIST,
            &arena,
            &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_OK);
    EXPECT(responseHeader.msgLen > 0);

    Endpoint_WorkspaceList_Response response;
    Endpoint_WorkspaceList_ResponseDeserializer(&response, &responseData, &arena);

    EXPECT_EQ(response.workspaceIds.len, 2);
    EXPECT_EQ(response.workspaceNames.len, response.workspaceIds.len);
    EXPECT_EQ(response.workspacePaths.len, response.workspaceIds.len);

    EXPECT_EQ(response.workspaceIds.data[0], 0);
    EXPECT_EQ(response.workspaceIds.data[1], 1);

    EXPECT(Str_EqStr(response.workspaceNames.data[0], Str_FromCStr("workspace_one")));
    EXPECT(Str_EqStr(response.workspaceNames.data[1], Str_FromCStr("workspace_two")));

    EXPECT(Str_EqCStr(response.workspacePaths.data[0], projectDir1));
    EXPECT(Str_EqCStr(response.workspacePaths.data[1], projectDir2));

    ServerContext_Destroy();
}

TEST(WorkspaceListTest_CallEndpoint_Empty)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_WorkspaceList_Create());

    Endpoint_WorkspaceList_Request request = {
        .__dummy = 0,
    };
    DataStream requestData = DataStream_New();
    Endpoint_WorkspaceList_RequestSerializer(&request, &requestData, &arena);

    DataStream responseData = EndpointRegistry_CallEndpoint(
            &endpointRegistry,
            ENDPOINT_ID_WORKSPACE_LIST,
            &arena,
            &requestData);
    ResponseHeader responseHeader = ResponseHeader_Deserialize(&responseData);

    EXPECT_EQ(responseHeader.returnCode, RETURN_CODE_OK);

    Endpoint_WorkspaceList_Response response;
    Endpoint_WorkspaceList_ResponseDeserializer(&response, &responseData, &arena);

    EXPECT_EQ(response.workspaceIds.len, 0);
    EXPECT_EQ(response.workspaceNames.len, 0);
    EXPECT_EQ(response.workspacePaths.len, 0);

    ServerContext_Destroy();
}
