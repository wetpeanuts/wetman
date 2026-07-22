#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/my_socket"

int main(void)
{
    int fd;
    struct sockaddr_un addr;
    char buf[128];

    fd = socket(AF_UNIX, SOCK_STREAM, 0);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    connect(fd, (struct sockaddr *)&addr, sizeof(addr));

    write(fd, "hello\n", 6);

    int n = read(fd, buf, sizeof(buf)-1);
    buf[n] = 0;

    printf("Reply: %s", buf);

    close(fd);
}
