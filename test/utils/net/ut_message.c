#include <wetman/utils/test/macro.h>
#include <wetman/utils/net/message.h>


TEST(MessageTest_RequestHeader_SerializeDeserialize)
{
    Arena      arena      = Arena_New();
    DataStream dataStream = DataStream_New();

    RequestHeader requestHeader = {
        .endpointId = 42,
        .msgLen     = 24,
    };

    RequestHeader_Serialize(&requestHeader, &dataStream, &arena);
    ASSERT_EQ(dataStream.__data.len, REQUEST_HEADER_SERIALIZED_LEN);

    RequestHeader expectedRequestHeader = RequestHeader_Deserialize(&dataStream);

    EXPECT_EQ(requestHeader.endpointId, expectedRequestHeader.endpointId);
    EXPECT_EQ(requestHeader.msgLen, expectedRequestHeader.msgLen);
}

TEST(MessageTest_ResponseHeader_SerializeDeserialize)
{
    Arena      arena      = Arena_New();
    DataStream dataStream = DataStream_New();

    ResponseHeader responseHeader = {
        .returnCode = 42,
        .msgLen     = 24,
    };

    ResponseHeader_Serialize(&responseHeader, &dataStream, &arena);
    ASSERT_EQ(dataStream.__data.len, RESPONSE_HEADER_SERIALIZED_LEN);

    ResponseHeader expectedResponseHeader = ResponseHeader_Deserialize(&dataStream);

    EXPECT_EQ(responseHeader.returnCode, expectedResponseHeader.returnCode);
    EXPECT_EQ(responseHeader.msgLen, expectedResponseHeader.msgLen);
}
