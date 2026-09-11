#include <wetman/utils/net/client/unix_client.h>
#include <wetman/utils/net/endpoint_registry.h>
#include <wetman/utils/net/macro.h>
#include <wetman/utils/net/server.h>
#include <wetman/utils/test/macro.h>

#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "../../../shared/endpoint/id.h"
#include "../../../shared/endpoint/echo_fd.h"

ENDPOINT_DECLARE_CLIENT(TestEndpointEchoFd)


TEST(UnixClientTest_EchoFd)
{
    char sockPath[128];
    snprintf(sockPath, sizeof(sockPath), "./unix_client_echo_fd.sock");
    unlink(sockPath);

    pid_t serverPid = fork();
    ASSERT_NE(serverPid, -1);

    if (serverPid == 0) {
        EndpointRegistry endpointRegistry = EndpointRegistry_New();
        EndpointRegistry_RegisterEndpoint(&endpointRegistry, TestEndpointEchoFd_Create());
        Server_Run(sockPath, &endpointRegistry);
        _exit(0);
    }

    sleep(1);

    Client client = UnixClient_Connect(sockPath);

    int fd = CREATE_TMP_FILE(O_RDWR | O_CREAT | O_TRUNC);
    ASSERT_NE(fd, -1);
    const char* payload = "file-descriptors-over-unix-sockets";
    ASSERT_EQ((i32)write(fd, payload, strlen(payload)), (i32)strlen(payload));
    lseek(fd, 0, SEEK_SET);

    TestEndpointEchoFd_Request request = {
        .fd = FileDescriptor_New(fd),
    };
    TestEndpointEchoFd_Response response = {
        .fd = FileDescriptor_New(FILE_DESCRIPTOR_INVALID),
    };

    Arena arena = Arena_New();
    ReturnCode returnCode = TestEndpointEchoFd_Call(&client, &request, &response, &arena);

    EXPECT_EQ(returnCode, RETURN_CODE_OK);
    EXPECT_NE(response.fd.fd, FILE_DESCRIPTOR_INVALID);

    if (response.fd.fd != FILE_DESCRIPTOR_INVALID) {
        char buf[64] = {0};
        isize n = read(response.fd.fd, buf, sizeof(buf) - 1);
        ASSERT_NE(n, -1);
        EXPECT_EQ(n, (isize)strlen(payload));
        EXPECT_EQ(strncmp(buf, payload, strlen(payload)), 0);
        close(response.fd.fd);
    }

    client.disconnect(&client);
    Arena_Free(&arena);
    close(fd);

    kill(serverPid, SIGKILL);
    waitpid(serverPid, NULL, 0);
    unlink(sockPath);
}
