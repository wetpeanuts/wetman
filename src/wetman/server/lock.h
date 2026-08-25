#ifndef WETMAN_SERVER_LOCK_H
#define WETMAN_SERVER_LOCK_H

#include <wetman/utils/data_struct/str.h>
#include <stdbool.h>

bool ServerLock_Acquire(Str lockFilePath);
void ServerLock_Release(void);

#endif // WETMAN_SERVER_LOCK_H
