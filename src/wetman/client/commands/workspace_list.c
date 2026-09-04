#include <wetman/client/commands/workspace_list.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/workspace_list.h>
#include <wetman/utils/data_struct/str.h>

#include <stdio.h>


static i32 __Command_WorkspaceList_Handler(const Args* args, Arena* arena)
{
    (void)args;

    Client client = globalClientContext.client;

    Endpoint_WorkspaceList_Request request = { 0 };
    Endpoint_WorkspaceList_Response response = { 0 };

    ReturnCode returnCode = Endpoint_WorkspaceList_Call(&client, &request, &response, arena);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Workspace list failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    if (response.workspaceIds.len == 0) {
        printf("No workspaces found\n");
        return 0;
    }

    Str idHeader    = Str_FromCStr("ID");
    Str nameHeader  = Str_FromCStr("Name");
    Str pathHeader  = Str_FromCStr("Path");

    Str* idValues   = (Str*)Arena_Alloc(arena, response.workspaceIds.len * sizeof(Str));
    Str* nameValues = response.workspaceNames.data;
    Str* pathValues = response.workspacePaths.data;

    usize maxIdLen   = idHeader.len;
    usize maxNameLen = nameHeader.len;
    usize maxPathLen = pathHeader.len;

    for (usize i = 0; i < response.workspaceIds.len; i++) {
        idValues[i] = Str_FromU64(response.workspaceIds.data[i], arena);

        if (idValues[i].len > maxIdLen) {
            maxIdLen = idValues[i].len;
        }
        if (nameValues[i].len > maxNameLen) {
            maxNameLen = nameValues[i].len;
        }
        if (pathValues[i].len > maxPathLen) {
            maxPathLen = pathValues[i].len;
        }
    }

    printf("%-*s  %-*s  %-*s\n", (i32)maxIdLen, "ID",
            (i32)maxNameLen, "Name", (i32)maxPathLen, "Path");

    for (usize i = 0; i < maxIdLen; i++) {
        putchar('-');
    }
    printf("  ");
    for (usize i = 0; i < maxNameLen; i++) {
        putchar('-');
    }
    printf("  ");
    for (usize i = 0; i < maxPathLen; i++) {
        putchar('-');
    }
    printf("\n");

    for (usize i = 0; i < response.workspaceIds.len; i++) {
        printf("%-*.*s  %-*.*s  %-*.*s\n",
                (i32)maxIdLen, (i32)idValues[i].len, idValues[i].data,
                (i32)maxNameLen, (i32)nameValues[i].len, nameValues[i].data,
                (i32)maxPathLen, (i32)pathValues[i].len, pathValues[i].data);
    }

    return 0;
}

Command Command_WorkspaceList_Create(Arena* arena)
{
    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 2);
    prefixData[0] = Str_FromCStr("workspace");
    prefixData[1] = Str_FromCStr("list");
    SliceStr prefix = SliceStr_FromData(prefixData, 2);

    Command cmd = {
        .prefix  = prefix,
        .args    = { .len = 0 },
        .handler = __Command_WorkspaceList_Handler,
    };

    return cmd;
}
