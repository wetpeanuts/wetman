#ifndef WETMAN_UTILS_CALLBACK_H
#define WETMAN_UTILS_CALLBACK_H

#include <wetman/utils/mem/arena.h>


typedef void(*CallbackHandler)(Arena*, void*);
typedef void(*CallbackHandlerNoArgs)(Arena*);

typedef struct Callback {
    CallbackHandler handler;
    void*           payload;
    Arena           arena;
} Callback;


Callback Callback_WithNoArgs(CallbackHandlerNoArgs callbackHandler);

void Callback_Invoke(Callback callback);

#endif // WETMAN_UTILS_CALLBACK_H 
