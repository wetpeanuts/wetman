#include <wetman/utils/net/server.h>

#include <wetman/utils/mem/arena.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

int Server_Run(const char* socketPath, EndpointRegistry* endpointRegistry)
{
    (void)endpointRegistry;

    int fdServer, fdClient;
    struct sockaddr_un addr;
    char buf[256];

    // remove old socket file
    unlink(socketPath);

    fdServer = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fdServer < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath, sizeof(addr.sun_path) - 1);

    if (bind(fdServer,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(fdServer, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("Listening on %s\n", socketPath);

    while (1) {
        fdClient = accept(fdServer, NULL, NULL);

        if (fdClient < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");

        while (1) {
            ssize_t n = read(fdClient, buf, sizeof(buf) - 1);

            if (n <= 0)
                break;

            buf[n] = '\0';

            printf("Received: %s\n", buf);

            const char *reply = "OK\n";
            write(fdClient, reply, strlen(reply));
        }

        close(fdClient);
        printf("Client disconnected\n");
    }

    close(fdServer);
    unlink(socketPath);

    return 0;
}

