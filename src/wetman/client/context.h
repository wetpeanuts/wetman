#ifndef WETMAN_CLIENT_CONTEXT_H
#define WETMAN_CLIENT_CONTEXT_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/net/client/client.h>


typedef struct {
    Arena  arena;
    Client client;
} ClientContext;


void ClientContext_Init(void);
void ClientContext_Destroy(void);

extern ClientContext globalClientContext;

#endif // WETMAN_CLIENT_CONTEXT_H

