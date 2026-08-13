#ifndef WETMAN_UTILS_NET_CLIENT_UNIX_CLIENT_H
#define WETMAN_UTILS_NET_CLIENT_UNIX_CLIENT_H

#include <wetman/utils/net/client/client.h>

// Creates a client instance that handles requests
// over unix socket.
Client UnixClient_Connect(const char* socketPath);

#endif // WETMAN_UTILS_NET_CLIENT_UNIX_CLIENT_H
