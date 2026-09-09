#include <wetman/utils/data_stream.h>
#include <wetman/utils/macro.h>
#include <wetman/utils/net/fd_stream.h>
#include <wetman/utils/net/message.h>
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

    EXPECT_EQ(FdStream_Push(&fdStream, FileDescriptor_New(1), &arena), 0);
    EXPECT_EQ(FdStream_Push(&fdStream, FileDescriptor_New(2), &arena), 0);
    EXPECT_EQ(FdStream_Push(&fdStream, FileDescriptor_New(3), &arena), 0);
    EXPECT_EQ(FdStream_Count(&fdStream), 3);

    EXPECT_EQ(FdStream_Pop(&fdStream).fd, 1);
    EXPECT_EQ(FdStream_Pop(&fdStream).fd, 2);
    EXPECT_EQ(FdStream_Count(&fdStream), 1);
    EXPECT_EQ(FdStream_Pop(&fdStream).fd, 3);
    EXPECT_EQ(FdStream_Count(&fdStream), 0);

    // Popping an empty stream returns the invalid sentinel
    EXPECT_EQ(FdStream_Pop(&fdStream).fd, FILE_DESCRIPTOR_INVALID);

    Arena_Free(&arena);
}

TEST(FdStreamTest_PassFdsOverSocketpair_RequestDirection)
{
    Arena arena = Arena_New();

    int fds[2] = {0, 0};
    ASSERT_EQ(socketpair(AF_UNIX, SOCK_STREAM, 0, fds), 0);
    int sockA = fds[0];
    int sockB = fds[1];

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

    // Send fds + a small byte body over sockA (mimicking a client request)
    Message sendMessage = Message_New();
    DataStream_PushStr(&sendMessage.bodyStream, Str_FromCStr("hello"), &arena);
    FdStream_Push(&sendMessage.fdStream, FileDescriptor_New(fileA), &arena);
    FdStream_Push(&sendMessage.fdStream, FileDescriptor_New(fileB), &arena);

    Message_Write(&sendMessage, sockA);
    EXPECT_EQ(sendMessage.bodyStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Receive on sockB (mimicking the server)
    Message recvMessage = Message_Read(sockB, &arena, sendMessage.bodyStream.__data.len);
    EXPECT_EQ(recvMessage.bodyStream.lastResult, DATA_STREAM_RESULT_SUCCESS);
    EXPECT_EQ(FdStream_Count(&recvMessage.fdStream), 2);

    // Received fds must reference the same open file descriptions
    Str receivedStr = DataStream_PopStr(&recvMessage.bodyStream);
    EXPECT(Str_EqCStr(receivedStr, "hello"));

    int fdA = FdStream_Pop(&recvMessage.fdStream).fd;
    int fdB = FdStream_Pop(&recvMessage.fdStream).fd;
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
    Message sendMessage = Message_New();
    DataStream_PushStr(&sendMessage.bodyStream, Str_FromCStr("resp"), &arena);
    FdStream_Push(&sendMessage.fdStream, FileDescriptor_New(fileA), &arena);

    Message_Write(&sendMessage, sockA);
    EXPECT_EQ(sendMessage.bodyStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Receive on sockB (mimicking the client)
    Message recvMessage = Message_Read(sockB, &arena, sendMessage.bodyStream.__data.len);
    EXPECT_EQ(recvMessage.bodyStream.lastResult, DATA_STREAM_RESULT_SUCCESS);
    EXPECT_EQ(FdStream_Count(&recvMessage.fdStream), 1);

    Str receivedStr = DataStream_PopStr(&recvMessage.bodyStream);
    EXPECT(Str_EqCStr(receivedStr, "resp"));

    int fdA = FdStream_Pop(&recvMessage.fdStream).fd;
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
