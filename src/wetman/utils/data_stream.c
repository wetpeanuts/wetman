#include <wetman/utils/data_stream.h>

#include <wetman/utils/macro.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define __DATA_STREAM_BUF_LEN 256

typedef enum {
    DATA_TYPE_I32,
    DATA_TYPE_U32,
    DATA_TYPE_STR,
} __DataType;


DataStream DataStream_New(void)
{
    DataStream dataStream = {
        .lastResult = DATA_STREAM_RESULT_SUCCESS,
        .__data     = Str_FromCStr(""),
    };
    return dataStream;
}

DataStream DataStream_WithData(DataSlice data)
{
    DataStream dataStream = {
        .lastResult = DATA_STREAM_RESULT_SUCCESS,
        .__data     = data,
    };
    return dataStream;
}

DataStream DataStream_Read(int fd, Arena* arena)
{
    DataSlice data = Str_FromCStr("");
    char buf[__DATA_STREAM_BUF_LEN];

    while (TRUE) {
        isize chunkLen = read(fd, buf, __DATA_STREAM_BUF_LEN);
        if (chunkLen <= 0) {
            break;
        }

        DataSlice dataChunk = {
            .data = buf,
            .len  = chunkLen,
        };
        data = Str_Concat(data, dataChunk, arena);
    }

    return DataStream_WithData(data);
}

void DataStream_Write(DataStream* dataStream, int fd)
{
    isize writtenLen = write(fd, dataStream->__data.data, dataStream->__data.len);
    if (writtenLen != (isize)dataStream->__data.len) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_WRITE;
        return;
    }

    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

void DataStream_PushI32(DataStream* dataStream, i32 value, Arena* arena)
{
    usize const dataLen = sizeof(i32) + sizeof(i32);
    i32* data = (i32*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *data = DATA_TYPE_I32;
    *(data + 1) = value;

    DataSlice serializedValue = {
        .data = (char*)data,
        .len  = dataLen,
    };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

i32 DataStream_PopI32(DataStream* dataStream)
{
    usize const dataLen = sizeof(i32) + sizeof(i32);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return 0;
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_I32) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return 0;
    }

    i32 value = *(((i32*)dataStream->__data.data) + 1);

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushU32(DataStream* dataStream, u32 value, Arena* arena)
{
    usize const dataLen = sizeof(i32) + sizeof(u32);
    i32* data = (i32*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *data = DATA_TYPE_U32;
    *((u32*)(((i32*)data) + 1)) = value;

    DataSlice serializedValue = {
        .data = (char*)data,
        .len  = dataLen,
    };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

u32 DataStream_PopU32(DataStream* dataStream)
{
    usize const dataLen = sizeof(i32) + sizeof(u32);
    if (dataStream->__data.len < sizeof(u32)) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return 0;
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_U32) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return 0;
    }

    u32 value = *((u32*)(((i32*)dataStream->__data.data) + 1));

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushStr(DataStream* dataStream, Str value, Arena* arena)
{
    usize const dataLen = sizeof(i32) + sizeof(usize) + value.len;
    i32* data = (i32*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *data = DATA_TYPE_STR;
    *((usize*)(((i32*)data) + 1)) = value.len;

    strncpy(((char*)data) + sizeof(i32) + sizeof(usize), value.data, value.len);

    DataSlice serializedValue = {
        .data = (char*)data,
        .len  = dataLen,
    };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

Str DataStream_PopStr(DataStream* dataStream)
{
    usize const dataHeaderLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < dataHeaderLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return Str_FromCStr("");
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_STR) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return Str_FromCStr("");
    }

    usize len = *((usize*)(((i32*)dataStream->__data.data) + 1));

    const usize dataLen = dataHeaderLen + len;
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return Str_FromCStr("");
    }

    Str value = {
        .data = dataStream->__data.data + dataHeaderLen,
        .len  = len,
    };

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}
