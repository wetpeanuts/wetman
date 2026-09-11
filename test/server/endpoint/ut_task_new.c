#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#include <wetman/server/endpoint/task_new.h>
#include <wetman/server/endpoint/workspace_init.h>
#include <wetman/server/context.h>
#include <wetman/server/persistence/task_state.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>
#include <wetman/utils/filesystem.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/message.h>
#include <wetman/utils/test/macro.h>

#include "utils.h"


TEST(TaskNewTest_CallEndpoint_Success)
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

    EXPECT_EQ(InitTestWorkspace(&endpointRegistry, &arena, projectDir, "test_project"), RETURN_CODE_OK);

    Endpoint_TaskNew_Response firstResponse = {0};
    Endpoint_TaskNew_Response secondResponse = {0};

    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "test_task", &firstResponse), RETURN_CODE_OK);
    EXPECT_EQ(firstResponse.taskId, 0);

    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 0, "second_task", &secondResponse), RETURN_CODE_OK);
    EXPECT_EQ(secondResponse.taskId, 1);

    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            Str_FromCStr("0"),
            &arena);

    Str firstTaskDir = FS_PathJoin(workspaceDir, Str_FromCStr("tasks/0"), &arena);
    EXPECT(FS_CheckExists(firstTaskDir));

    Str secondTaskDir = FS_PathJoin(workspaceDir, Str_FromCStr("tasks/1"), &arena);
    EXPECT(FS_CheckExists(secondTaskDir));

    Str firstTaskShPath = FS_PathJoin(firstTaskDir, Str_FromCStr("task.sh"), &arena);
    EXPECT(FS_CheckExists(firstTaskShPath));

    struct stat st;
    char taskShPathBuf[PATH_MAX];
    i32 n = snprintf(
            taskShPathBuf,
            sizeof(taskShPathBuf),
            "%.*s",
            (i32)firstTaskShPath.len,
            firstTaskShPath.data);
    ASSERT(n > 0 && (usize)n < sizeof(taskShPathBuf));
    ASSERT_EQ(stat(taskShPathBuf, &st), 0);
    EXPECT(st.st_mode & S_IRWXU);

    Str firstTaskStatePath = FS_PathJoin(firstTaskDir, Str_FromCStr("task.wmtsk"), &arena);
    EXPECT(FS_CheckExists(firstTaskStatePath));

    PersistenceStatus readStatus;
    TaskState firstTaskState = TaskState_Read(firstTaskStatePath, &arena, &readStatus);
    EXPECT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(firstTaskState.taskId, 0);
    EXPECT(Str_EqCStr(firstTaskState.taskName, "test_task"));

    Str secondTaskStatePath = FS_PathJoin(secondTaskDir, Str_FromCStr("task.wmtsk"), &arena);
    EXPECT(FS_CheckExists(secondTaskStatePath));

    TaskState secondTaskState = TaskState_Read(secondTaskStatePath, &arena, &readStatus);
    EXPECT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(secondTaskState.taskId, 1);
    EXPECT(Str_EqCStr(secondTaskState.taskName, "second_task"));

    Str configPath = FS_PathJoin(workspaceDir, Str_FromCStr("workspace.wmwscfg"), &arena);
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, &arena, &readStatus);
    EXPECT_EQ(readStatus, PERSISTENCE_STATUS_OK);
    EXPECT_EQ(config.nextTaskId, 2);
    EXPECT(Str_EqCStr(config.workspacePath, projectDir));

    ServerContext_Destroy();
}

TEST(TaskNewTest_CallEndpoint_WorkspaceNotFound)
{
    Arena arena = Arena_New();
    EndpointRegistry endpointRegistry = EndpointRegistry_New();

    char serverWorkingDir[256];
    CREATE_TMP_DIR(serverWorkingDir);

    ServerContext_Init(arena, Str_FromCStr(serverWorkingDir));

    EndpointRegistry_RegisterEndpoint(&endpointRegistry, Endpoint_TaskNew_Create());

    Endpoint_TaskNew_Response response = {0};
    EXPECT_EQ(CreateTask(&endpointRegistry, &arena, 999, "task", &response),
            RETURN_CODE_INTERNAL_ENDPOINT_ERROR);

    ServerContext_Destroy();
}

