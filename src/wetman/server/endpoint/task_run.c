#include <wetman/server/endpoint/task_run.h>

#include <wetman/server/context.h>
#include <wetman/shared/endpoint/id.h>
#include <wetman/shared/persistence/workspace_config.h>

#include <wetman/utils/net/macro.h>
#include <wetman/utils/filesystem.h>

#include <limits.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>


static i32 __Endpoint_TaskRun_StrToCStr(Str str, char* buf, usize bufLen)
{
    if (str.len >= bufLen) {
        return -1;
    }
    memcpy(buf, str.data, str.len);
    buf[str.len] = '\0';
    return 0;
}

// Runs in the forked child, after stdout/stderr have been redirected to the
// pipe. Closes every inherited descriptor (the server's listen socket, other
// clients, received request fds, ...) so the task cannot outlive or tamper
// with the server's I/O.
static void __Endpoint_TaskRun_CloseInheritedFds(long maxFd)
{
    for (int fd = STDERR_FILENO + 1; fd < (int)maxFd; fd++) {
        close(fd);
    }
}


ReturnCode Endpoint_TaskRun(
        Endpoint_TaskRun_Request*  request,
        Endpoint_TaskRun_Response* response,
        Arena*                     arena)
{
    response->stdoutFd = FileDescriptor_New(FILE_DESCRIPTOR_INVALID);

    Str workspaceDirName = Str_FromU64((u64)request->workspaceId, arena);
    Str workspaceDir = FS_PathJoin(
            globalServerContext.workspacesPath,
            workspaceDirName,
            arena);

    if (!FS_CheckExists(workspaceDir)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str configPath = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("workspace.wmwscfg"),
            arena);

    PersistenceStatus status;
    WorkspaceConfig config = WorkspaceConfig_Read(configPath, arena, &status);
    if (status != PERSISTENCE_STATUS_OK) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    Str tasksDir = FS_PathJoin(
            workspaceDir,
            Str_FromCStr("tasks"),
            arena);
    Str taskDirName = Str_FromU64((u64)request->taskId, arena);
    Str taskDir = FS_PathJoin(tasksDir, taskDirName, arena);
    Str taskShPath = FS_PathJoin(taskDir, Str_FromCStr("task.sh"), arena);

    if (!FS_CheckExists(taskShPath)) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    char workspacePathBuf[PATH_MAX];
    char taskShPathBuf[PATH_MAX];
    if (__Endpoint_TaskRun_StrToCStr(config.workspacePath, workspacePathBuf, sizeof(workspacePathBuf)) != 0) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }
    if (__Endpoint_TaskRun_StrToCStr(taskShPath, taskShPathBuf, sizeof(taskShPathBuf)) != 0) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    int pipeFds[2];
    if (pipe(pipeFds) == -1) {
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    long maxFd = sysconf(_SC_OPEN_MAX);
    if (maxFd < 0) {
        maxFd = 1024;
    }
    if (maxFd > 65536) {
        maxFd = 65536;
    }

    pid_t pid = fork();
    if (pid == -1) {
        close(pipeFds[0]);
        close(pipeFds[1]);
        return RETURN_CODE_INTERNAL_ENDPOINT_ERROR;
    }

    if (pid == 0) {
        // Child: run the task with its output going into the pipe.
        dup2(pipeFds[1], STDOUT_FILENO);
        dup2(pipeFds[1], STDERR_FILENO);
        __Endpoint_TaskRun_CloseInheritedFds(maxFd);

        // The server ignores SIGPIPE; restore the default for the task so it
        // dies if its output consumer goes away.
        signal(SIGPIPE, SIG_DFL);

        if (chdir(workspacePathBuf) != 0) {
            _exit(127);
        }

        execlp("sh", "sh", taskShPathBuf, (char*)NULL);
        _exit(127);
    }

    // Parent (server): keep only the read end and hand it to the client.
    close(pipeFds[1]);
    response->stdoutFd = FileDescriptor_New(pipeFds[0]);

    return RETURN_CODE_OK;
}

ENDPOINT_IMPL_SERVER(ENDPOINT_ID_TASK_RUN, Endpoint_TaskRun)
