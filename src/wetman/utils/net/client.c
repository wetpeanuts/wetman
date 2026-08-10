#include <wetman/utils/net/client.h>


Client Client_New(const char* socketPath)
{
    Client client = {
        .socketPath = socketPath,
    };

    return client;
}
