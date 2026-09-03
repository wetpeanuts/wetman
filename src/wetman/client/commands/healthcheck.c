#include <wetman/client/commands/healthcheck.h>

#include <wetman/client/context.h>
#include <wetman/client/endpoint/health_check.h>

#include <stdio.h>


static i32 __Command_HealthCheck_Handler(const Args* args, Arena* arena)
{
    (void)args;
    (void)arena;

    Client client = globalClientContext.client;

    Endpoint_HealthCheck_Request request = { 0 };
    Endpoint_HealthCheck_Response response = { 0 };

    ReturnCode returnCode = Endpoint_HealthCheck_Call(&client, &request, &response);

    if (returnCode != RETURN_CODE_OK) {
        fprintf(stderr,
                "Health check failed with status code: %d\n",
                (i32)returnCode);
        return (i32)returnCode;
    }

    printf("OK\n");

    return 0;
}

Command Command_HealthCheck_Create(Arena* arena)
{
    Str* prefixData = (Str*)Arena_Alloc(arena, sizeof(Str) * 1);
    prefixData[0] = Str_FromCStr("healthcheck");
    SliceStr prefix = SliceStr_FromData(prefixData, 1);

    Command cmd = {
        .prefix  = prefix,
        .args    = { .len = 0 },
        .handler = __Command_HealthCheck_Handler,
    };

    return cmd;
}
