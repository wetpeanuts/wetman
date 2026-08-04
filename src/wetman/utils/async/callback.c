#include <wetman/utils/async/callback.h>

#include <stddef.h>

typedef struct {
    CallbackHandlerNoArgs callbackHandler;
} __CallbackPayloadNoArgs;

void __callbackHandlerNoArgs(Arena* arena, void* payload)
{
    __CallbackPayloadNoArgs* payloadNoArgs = (__CallbackPayloadNoArgs*)payload;
    payloadNoArgs->callbackHandler(arena);
}

Callback Callback_WithNoArgs(CallbackHandlerNoArgs callbackHandler)
{
    Arena arena = Arena_New();

    __CallbackPayloadNoArgs* payload = (__CallbackPayloadNoArgs*)Arena_Alloc(
            &arena, sizeof(__CallbackPayloadNoArgs));
    payload->callbackHandler = callbackHandler;

    Callback callback = {
        .handler = __callbackHandlerNoArgs,
        .payload = (void*)payload,
        .arena   = arena,
    };

    return callback;
}

void Callback_Invoke(Callback* callback)
{
    callback->handler(&callback->arena, callback->payload);

    Arena_Free(&callback->arena);
    callback->handler = NULL;
    callback->payload = NULL;
}

int Callback_IsValid(const Callback* callback)
{
    return Arena_IsValid(&callback->arena)
            && callback->handler != NULL;
}

