#ifndef WETMAN_UTILS_SERIALIZATION_H
#define WETMAN_UTILS_SERIALIZATION_H

#include <wetman/utils/data_struct/str.h>
#include <wetman/utils/mem/arena.h>
#include <wetman/utils/type.h>


typedef Str DataSlice;

typedef enum
{
    DATA_STREAM_RESULT_SUCCESS,
    DATA_STREAM_RESULT_FAILED_ALLOCATE_MEMORY,
    DATA_STREAM_RESULT_WRONG_VALUE_TYPE,
    DATA_STREAM_RESULT_WRONG_VALUE_FORMAT,
    DATA_STREAM_RESULT_FAILED_WRITE,
    DATA_STREAM_RESULT_FAILED_READ,
} DataStreamResult;

typedef struct {
    DataStreamResult lastResult;
    DataSlice        __data;
} DataStream;


DataStream DataStream_New(void);
DataStream DataStream_WithData(DataSlice data);

DataStream DataStream_Read(int fd, Arena* arena, isize maxLen);
void DataStream_Write(DataStream* dataStream, int fd);

void DataStream_PushI32(DataStream* dataStream, i32 value, Arena* arena);
i32 DataStream_PopI32(DataStream* dataStream);

void DataStream_PushU32(DataStream* dataStream, u32 value, Arena* arena);
u32 DataStream_PopU32(DataStream* dataStream);

void DataStream_PushStr(DataStream* dataStream, Str value, Arena* arena);
Str DataStream_PopStr(DataStream* dataStream);

#endif // WETMAN_UTILS_SERIALIZATION_H
