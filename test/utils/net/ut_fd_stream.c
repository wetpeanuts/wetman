#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/net/fd_stream.h>
#include <wetman/utils/test/macro.h>

#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>


TEST(FdStreamTest_PushPopCount)
{
    FdStream fdStream = FdStream_New();
    Arena arena = Arena_New();

    ASSERT_EQ(FdStream_Count(&fdStream), 0);

    EXPECT_EQ(FdStream_Push(&fdStream, 1, &arena), 0);
    EXPECT_EQ(FdStream_Push(&fdStream, 2, &arena), 0);
    EXPECT_EQ(FdStream_Push(&fdStream, 3, &arena), 0);
    EXPECT_EQ(FdStream_Count(&fdStream), 3);

    EXPECT_EQ(FdStream_Pop(&fdStream), 1);
    EXPECT_EQ(FdStream_Pop(&fdStream), 2);
    EXPECT_EQ(FdStream_Count(&fdStream), 1);
    EXPECT_EQ(FdStream_Pop(&fdStream), 3);
    EXPECT_EQ(FdStream_Count(&fdStream), 0);

    // Popping an empty stream returns the invalid sentinel
    EXPECT_EQ(FdStream_Pop(&fdStream), -1);

    Arena_Free(&arena);
}

TEST(FdStreamTest_PassFdsOverSocketpair_RequestDirection)
{
    Arena arena = Arena_New();

    int sockA = -1;
    int sockB = -1;
    {
        int fds[2] = {0, 0};
        ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
        sockA = fds[0];
        sockB = fds[1];
    }

    // Two tmp files with distinct contents to send over the wire
    int fileA = CREATE_TMP_FILE(O_RDWR | O_CREAT | O_TRUNC);
    int fileB = CREATE_TMP_FILE(O_RDWR | O_CREAT | O_TRUNC);
    ASSERT_NE(fileA, -1);
    ASSERT_NE(fileB, -1);

    const char* payloadA = "payload-one";
    const char* payloadB = "payload-two";
    ASSERT_EQ((i32)write(fileA, payloadA, strlen(payloadA)), (i32)strlen(payloadA));
    ASSERT_EQ((i32)write(fileB, payloadB, strlen(payloadB)), (i32)strlen(payloadB));
    lseek(fileA, 0, SEEK_SET);
    lseek(fileB, 0, SEEK_SET);

    // Send fds + a small byte body over sockA
    FdStream sendFds = FdStream_New();
    ASSERT_EQ(FdStream_Push(&sendFds, fileA, &arena), 0);
    ASSERT_EQ(FdStream_Push(&sendFds, fileB, &arena), 0);

    DataStream body = DataStream_New();
    DataStream_PushStr(&body, Str_FromCStr("hello"), &arena);
    DataStream_WriteMsg(&body, sockA, &sendFds);
    EXPECT_EQ(body.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Receive on sockB
    FdStream recvFds = FdStream_New();
    DataStream recvBody = DataStream_ReadMsg(sockB, &arena, body.__data.len, &recvFds);
    EXPECT_EQ(recvBody.__data.len, body.__data.len);
    EXPECT_EQ(FdStream_Count(&recvFds), 2);

    // Received fds must reference the same open file descriptions
    Str receivedStr = DataStream_PopStr(&recvBody);
    EXPECT(Str_EqCStr(receivedStr, "hello"));

    int fdA = FdStream_Pop(&recvFds);
    int fdB = FdStream_Pop(&recvFds);
    ASSERT_NE(fdA, -1);
    ASSERT_NE(fdB, -1);

    char readBuf[32] = {0};
    ASSERT_EQ((i32)read(fdA, readBuf, sizeof(readBuf)), (i32)strlen(payloadA));
    EXPECT_EQ(strncmp(readBuf, payloadA, strlen(payloadA)), 0);
    memset(readBuf, 0, sizeof(readBuf));
    ASSERT_EQ((i32)read(fdB, readBuf, sizeof(readBuf)), (i32)strlen(payloadB));
    EXPECT_EQ(strncmp(readBuf, payloadB, strlen(payloadB)), 0);

    close(fdA);
    close(fdB);
    close(sockA);
    close(sockB);
    close(fileA);
    close(fileB);
    Arena_Free(&arena);
}

TEST(FdStreamTest_PassFdsOverSocketpair_ResponseDirection)
{
    Arena arena = Arena_New();

    int fds[2] = {0, 0};
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
    int sockA = fds[0];
    int sockB = fds[1];

    int fileA = CREATE_TMP_FILE(O_RDWR | O_CREAT | O_TRUNC);
    ASSERT_NE(fileA, -1);
    const char* payload = "server-owned-data";
    ASSERT_EQ((i32)write(fileA, payload, strlen(payload)), (i32)strlen(payload));
    lseek(fileA, 0, SEEK_SET);

    // Send fds + bytes from sockA (mimicking the server response direction)
    FdStream sendFds = FdStream_New();
    ASSERT_EQ(FdStream_Push(&sendFds, fileA, &arena), 0);

    DataStream body = DataStream_New();
    DataStream_PushStr(&body, Str_FromCStr("resp"), &arena);
    DataStream_WriteMsg(&body, sockA, &sendFds);
    EXPECT_EQ(body.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Receive on sockB (mimicking the client)
    FdStream recvFds = FdStream_New();
    DataStream recvBody = DataStream_ReadMsg(sockB, &arena, body.__data.len, &recvFds);
    EXPECT_EQ(recvBody.__data.len, body.__data.len);
    EXPECT_EQ(FdStream_Count(&recvFds), 1);

    Str receivedStr = DataStream_PopStr(&recvBody);
    EXPECT(Str_EqCStr(receivedStr, "resp"));

    int fdA = FdStream_Pop(&recvFds);
    ASSERT_NE(fdA, -1);

    char readBuf[32] = {0};
    ASSERT_EQ((i32)read(fdA, readBuf, sizeof(readBuf)), (i32)strlen(payload));
    EXPECT_EQ(strncmp(readBuf, payload, strlen(payload)), 0);

    close(fdA);
    close(sockA);
    close(sockB);
    close(fileA);
    Arena_Free(&arena);
}
