#ifndef WETMAN_UTILS_ASYNC_CALLBACK_H
#define WETMAN_UTILS_ASYNC_CALLBACK_H

#include <wetman/utils/mem/arena.h>

typedef struct Callback Callback;

typedef void(*CallbackHandler)(Callback*);

typedef enum {
    CALLBACK_ARENA_POLICY_FREE,
    CALLBACK_ARENA_POLICY_RESET,
    CALLBACK_ARENA_POLICY_DO_NOTHING,
} CallbackArenaPolicy;

typedef struct Callback {
    CallbackHandler     handler;
    void*               payload;
    Arena               arena;
    CallbackArenaPolicy arenaPolicy;
} Callback;


Callback Callback_New(
        CallbackHandler callbackHandler,
        void*           payload,
        Arena           arena);
Callback Callback_WithNoArgs(CallbackHandler callbackHandler);

void Callback_Invoke(Callback* callback);
int Callback_IsValid(const Callback* callback);


#define CALLBACK_DECLARE(HandlerName, PayloadType) \
    void __CallbackHandler_##HandlerName(Callback* callbackMeta) \
    { \
        PayloadType* payload = (PayloadType*)callbackMeta->payload; \
        HandlerName(payload); \
    }

#define CALLBACK_CREATE(HandlerName, payload, arena) \
    Callback_New( \
            __CallbackHandler_##HandlerName, \
            (void*)payload, \
            arena)

#endif // WETMAN_UTILS_ASYNC_CALLBACK_H 
