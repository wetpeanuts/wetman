#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define SOCKET_PATH "/tmp/my_socket"

int main(void)
{
    int server_fd, client_fd;
    struct sockaddr_un addr;
    char buf[256];

    unlink(SOCKET_PATH); // remove old socket file

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd,
             (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        return 1;
    }

    printf("Listening on %s\n", SOCKET_PATH);

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected\n");

        while (1) {
            ssize_t n = read(client_fd, buf, sizeof(buf) - 1);

            if (n <= 0)
                break;

            buf[n] = '\0';

            printf("Received: %s\n", buf);

            const char *reply = "OK\n";
            write(client_fd, reply, strlen(reply));
        }

        close(client_fd);
        printf("Client disconnected\n");
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
