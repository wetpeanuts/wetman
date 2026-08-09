#include <wetman/utils/test/macro.h>

#include <wetman/utils/data_stream.h>

#include <fcntl.h>


TEST(DataStream_PushPop_I32)
{
    Arena arena = Arena_New();
    i32 value1 = 42;
    i32 value2 = -42;

    DataStream dataStream = DataStream_New();

    DataStream_PushI32(&dataStream, value1, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    DataStream_PushI32(&dataStream, value2, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read different type
    {
        MAYBE_UNUSED u32 _ = DataStream_PopU32(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_TYPE);
    }

    i32 deserializedValue1 = DataStream_PopI32(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    i32 deserializedValue2 = DataStream_PopI32(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read empty data stream
    {
        MAYBE_UNUSED i32 _ = DataStream_PopI32(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_FORMAT);
    }

    EXPECT_EQ(deserializedValue1, value1);
    EXPECT_EQ(deserializedValue2, value2);

    Arena_Free(&arena);
}

TEST(DataStream_PushPop_U32)
{
    Arena arena = Arena_New();
    u32 value1 = 42;
    u32 value2 = 24;

    DataStream dataStream = DataStream_New();

    DataStream_PushU32(&dataStream, value1, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    DataStream_PushU32(&dataStream, value2, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read different type
    {
        MAYBE_UNUSED Str _ = DataStream_PopStr(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_TYPE);
    }

    u32 deserializedValue1 = DataStream_PopU32(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    u32 deserializedValue2 = DataStream_PopU32(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read empty data stream
    {
        MAYBE_UNUSED u32 _ = DataStream_PopU32(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_FORMAT);
    }

    EXPECT_EQ(deserializedValue1, value1);
    EXPECT_EQ(deserializedValue2, value2);

    Arena_Free(&arena);
}

TEST(DataStream_PushPop_Str)
{
    Arena arena = Arena_New();
    Str value1 = Str_FromCStr("Hello");
    Str value2 = Str_FromCStr("World!");

    DataStream dataStream = DataStream_New();

    DataStream_PushStr(&dataStream, value1, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    DataStream_PushStr(&dataStream, value2, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read different type
    {
        MAYBE_UNUSED i32 _ = DataStream_PopI32(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_TYPE);
    }

    Str deserializedValue1 = DataStream_PopStr(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    Str deserializedValue2 = DataStream_PopStr(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Try read empty data stream
    {
        MAYBE_UNUSED Str _ = DataStream_PopStr(&dataStream);
        ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_WRONG_VALUE_FORMAT);
    }

    EXPECT(Str_EqStr(deserializedValue1, value1));
    EXPECT(Str_EqStr(deserializedValue2, value2));

    Arena_Free(&arena);
}

TEST(DataStream_ReadWrite)
{
    i32 fdTmp = CREATE_TMP_FILE(O_RDWR | O_CREAT | O_TRUNC);

    Arena      arena      = Arena_New();
    DataStream dataStream = DataStream_New();

    Str value1 = Str_FromCStr("Hello");
    i32 value2 = 42;

    DataStream_PushStr(&dataStream, value1, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    DataStream_PushI32(&dataStream, value2, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    DataStream_Write(&dataStream, fdTmp);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    // Reset file offset after write
    ASSERT(lseek(fdTmp, 0, SEEK_SET) != (off_t)-1);

    dataStream = DataStream_Read(fdTmp, &arena);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    Str deserializedValue1 = DataStream_PopStr(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    i32 deserializedValue2 = DataStream_PopI32(&dataStream);
    ASSERT_EQ(dataStream.lastResult, DATA_STREAM_RESULT_SUCCESS);

    EXPECT(Str_EqStr(deserializedValue1, value1));
    EXPECT_EQ(deserializedValue2, value2);

    Arena_Free(&arena);
}
