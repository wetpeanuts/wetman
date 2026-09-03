#include <wetman/utils/data_stream.h>

#include <wetman/utils/data_struct/slice_i32.h>
#include <wetman/utils/data_struct/slice_u32.h>
#include <wetman/utils/data_struct/slice_u64.h>
#include <wetman/utils/data_struct/slice_i64.h>
#include <wetman/utils/data_struct/slice_str.h>
#include <wetman/utils/macro.h>

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#define __DATA_STREAM_BUF_LEN 256

typedef enum {
    DATA_TYPE_I32,
    DATA_TYPE_U32,
    DATA_TYPE_U64,
    DATA_TYPE_STR,
    DATA_TYPE_SLICE_I32,
    DATA_TYPE_SLICE_U32,
    DATA_TYPE_SLICE_U64,
    DATA_TYPE_SLICE_I64,
    DATA_TYPE_SLICE_STR,
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

DataStream DataStream_Read(int fd, Arena* arena, isize maxLen)
{
    DataSlice data = Str_FromCStr("");
    char buf[__DATA_STREAM_BUF_LEN];
    isize readLen = 0;

    while (readLen < maxLen) {
        isize chunkLen = read(fd, buf, MIN(__DATA_STREAM_BUF_LEN, maxLen - readLen));
        if (chunkLen <= 0) {
            break;
        }

        readLen += chunkLen;
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

void DataStream_Append(DataStream* dest, const DataStream* src, Arena* arena)
{
    dest->__data = Str_Concat(dest->__data, src->__data, arena);
    dest->lastResult = DATA_STREAM_RESULT_SUCCESS;
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

void DataStream_PushU64(DataStream* dataStream, u64 value, Arena* arena)
{
    usize const dataLen = sizeof(i32) + sizeof(u64);
    i32* data = (i32*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *data = DATA_TYPE_U64;
    *((u64*)(((i32*)data) + 1)) = value;

    DataSlice serializedValue = {
        .data = (char*)data,
        .len  = dataLen,
    };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

u64 DataStream_PopU64(DataStream* dataStream)
{
    usize const dataLen = sizeof(i32) + sizeof(u64);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return 0;
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_U64) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return 0;
    }

    u64 value = *((u64*)(((i32*)dataStream->__data.data) + 1));

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

void DataStream_PushSliceI32(
    DataStream* dataStream, SliceI32 value, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    usize const dataLen = headerLen + value.len * sizeof(i32);
    char* data = (char*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *((i32*)data) = DATA_TYPE_SLICE_I32;
    *((usize*)(data + sizeof(i32))) = value.len;
    if (value.len > 0) {
        memcpy(data + headerLen, value.data, value.len * sizeof(i32));
    }

    DataSlice serializedValue = { .data = data, .len = dataLen };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

SliceI32 DataStream_PopSliceI32(DataStream* dataStream)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < headerLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceI32_CreateEmpty();
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_SLICE_I32) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return SliceI32_CreateEmpty();
    }

    usize len = *((usize*)(dataStream->__data.data + sizeof(i32)));

    usize const dataLen = headerLen + len * sizeof(i32);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceI32_CreateEmpty();
    }

    SliceI32 value = SliceI32_FromData(
        (i32*)(dataStream->__data.data + headerLen), len);

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushSliceU32(
    DataStream* dataStream, SliceU32 value, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    usize const dataLen = headerLen + value.len * sizeof(u32);
    char* data = (char*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *((i32*)data) = DATA_TYPE_SLICE_U32;
    *((usize*)(data + sizeof(i32))) = value.len;
    if (value.len > 0) {
        memcpy(data + headerLen, value.data, value.len * sizeof(u32));
    }

    DataSlice serializedValue = { .data = data, .len = dataLen };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

SliceU32 DataStream_PopSliceU32(DataStream* dataStream)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < headerLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceU32_CreateEmpty();
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_SLICE_U32) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return SliceU32_CreateEmpty();
    }

    usize len = *((usize*)(dataStream->__data.data + sizeof(i32)));

    usize const dataLen = headerLen + len * sizeof(u32);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceU32_CreateEmpty();
    }

    SliceU32 value = SliceU32_FromData(
        (u32*)(dataStream->__data.data + headerLen), len);

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushSliceU64(
    DataStream* dataStream, SliceU64 value, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    usize const dataLen = headerLen + value.len * sizeof(u64);
    char* data = (char*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *((i32*)data) = DATA_TYPE_SLICE_U64;
    *((usize*)(data + sizeof(i32))) = value.len;
    if (value.len > 0) {
        memcpy(data + headerLen, value.data, value.len * sizeof(u64));
    }

    DataSlice serializedValue = { .data = data, .len = dataLen };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

SliceU64 DataStream_PopSliceU64(DataStream* dataStream)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < headerLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceU64_CreateEmpty();
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_SLICE_U64) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return SliceU64_CreateEmpty();
    }

    usize len = *((usize*)(dataStream->__data.data + sizeof(i32)));

    usize const dataLen = headerLen + len * sizeof(u64);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceU64_CreateEmpty();
    }

    SliceU64 value = SliceU64_FromData(
        (u64*)(dataStream->__data.data + headerLen), len);

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushSliceI64(
    DataStream* dataStream, SliceI64 value, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    usize const dataLen = headerLen + value.len * sizeof(i64);
    char* data = (char*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *((i32*)data) = DATA_TYPE_SLICE_I64;
    *((usize*)(data + sizeof(i32))) = value.len;
    if (value.len > 0) {
        memcpy(data + headerLen, value.data, value.len * sizeof(i64));
    }

    DataSlice serializedValue = { .data = data, .len = dataLen };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

SliceI64 DataStream_PopSliceI64(DataStream* dataStream)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < headerLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceI64_CreateEmpty();
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_SLICE_I64) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return SliceI64_CreateEmpty();
    }

    usize len = *((usize*)(dataStream->__data.data + sizeof(i32)));

    usize const dataLen = headerLen + len * sizeof(i64);
    if (dataStream->__data.len < dataLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceI64_CreateEmpty();
    }

    SliceI64 value = SliceI64_FromData(
        (i64*)(dataStream->__data.data + headerLen), len);

    dataStream->__data.data += dataLen;
    dataStream->__data.len -= dataLen;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return value;
}

void DataStream_PushSliceStr(
    DataStream* dataStream, SliceStr value, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);

    usize strDataLen = 0;
    for (usize i = 0; i < value.len; i++) {
        strDataLen += sizeof(usize) + value.data[i].len;
    }

    usize const dataLen = headerLen + strDataLen;
    char* data = (char*)Arena_Alloc(arena, dataLen);
    if (!data) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return;
    }

    *((i32*)data) = DATA_TYPE_SLICE_STR;
    *((usize*)(data + sizeof(i32))) = value.len;

    char* cursor = data + headerLen;
    for (usize i = 0; i < value.len; i++) {
        *((usize*)cursor) = value.data[i].len;
        cursor += sizeof(usize);
        memcpy(cursor, value.data[i].data, value.data[i].len);
        cursor += value.data[i].len;
    }

    DataSlice serializedValue = { .data = data, .len = dataLen };

    dataStream->__data = Str_Concat(dataStream->__data, serializedValue, arena);
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;
}

SliceStr DataStream_PopSliceStr(DataStream* dataStream, Arena* arena)
{
    usize const headerLen = sizeof(i32) + sizeof(usize);
    if (dataStream->__data.len < headerLen) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
        return SliceStr_CreateEmpty();
    }

    i32 valueType = *((i32*)dataStream->__data.data);
    if (valueType != DATA_TYPE_SLICE_STR) {
        dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_TYPE;
        return SliceStr_CreateEmpty();
    }

    usize sliceLen = *((usize*)(dataStream->__data.data + sizeof(i32)));

    char* cursor = dataStream->__data.data + headerLen;
    usize remaining = dataStream->__data.len - headerLen;

    Str* items = (Str*)Arena_Alloc(arena, sliceLen * sizeof(Str));
    if (!items) {
        dataStream->lastResult = DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY;
        return SliceStr_CreateEmpty();
    }

    for (usize i = 0; i < sliceLen; i++) {
        if (remaining < sizeof(usize)) {
            dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
            return SliceStr_CreateEmpty();
        }
        usize strLen = *((usize*)cursor);
        cursor += sizeof(usize);
        remaining -= sizeof(usize);

        if (remaining < strLen) {
            dataStream->lastResult = DATA_STREAM_RESULT_WRONG_VALUE_FORMAT;
            return SliceStr_CreateEmpty();
        }
        items[i] = (Str){ .data = cursor, .len = strLen };
        cursor += strLen;
        remaining -= strLen;
    }

    dataStream->__data.data = (char*)cursor;
    dataStream->__data.len = remaining;
    dataStream->lastResult = DATA_STREAM_RESULT_SUCCESS;

    return SliceStr_FromData(items, sliceLen);
}
