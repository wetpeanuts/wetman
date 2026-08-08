#include <wetman/utils/async/callback.h>

#include <stddef.h>

typedef struct {
    CallbackHandler callbackHandler;
} __CallbackPayloadNoArgs;

void __callbackHandlerNoArgs(Callback* callbackMeta)
{
    __CallbackPayloadNoArgs* payloadNoArgs = (__CallbackPayloadNoArgs*)callbackMeta->payload;
    payloadNoArgs->callbackHandler(callbackMeta);
}

Callback Callback_New(
        CallbackHandler callbackHandler,
        void*           payload,
        Arena           arena)
{
    Callback callback = {
        .handler     = callbackHandler,
        .payload     = payload,
        .arena       = arena,
        .arenaPolicy = CALLBACK_ARENA_POLICY_FREE,
    };

    return callback;
}

Callback Callback_WithNoArgs(CallbackHandler callbackHandler)
{
    Arena arena = Arena_New();

    __CallbackPayloadNoArgs* payload = (__CallbackPayloadNoArgs*)Arena_Alloc(
            &arena, sizeof(__CallbackPayloadNoArgs));
    payload->callbackHandler = callbackHandler;

    return Callback_New(
            __callbackHandlerNoArgs,
            (void*)payload,
            arena);
}

void Callback_Invoke(Callback* callback)
{
    callback->handler(callback);

    switch (callback->arenaPolicy) {
        case CALLBACK_ARENA_POLICY_FREE:
            Arena_Free(&callback->arena);
            break;
        case CALLBACK_ARENA_POLICY_RESET:
            Arena_Reset(&callback->arena);
            break;
        case CALLBACK_ARENA_POLICY_DO_NOTHING:
            break;
    }

    callback->handler = NULL;
    callback->payload = NULL;
}

int Callback_IsValid(const Callback* callback)
{
    return Arena_IsValid(&callback->arena)
            && callback->handler != NULL;
}

