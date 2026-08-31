#ifndef WETMAN_UTILS_SERIALIZATION_H
#define WETMAN_UTILS_SERIALIZATION_H

#include <wetman/utils/data_struct/slice_i32.h>
#include <wetman/utils/data_struct/slice_str.h>
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
void DataStream_Append(DataStream* dest, const DataStream* src, Arena* arena);

void DataStream_PushI32(DataStream* dataStream, i32 value, Arena* arena);
i32 DataStream_PopI32(DataStream* dataStream);

void DataStream_PushU32(DataStream* dataStream, u32 value, Arena* arena);
u32 DataStream_PopU32(DataStream* dataStream);

void DataStream_PushU64(DataStream* dataStream, u64 value, Arena* arena);
u64 DataStream_PopU64(DataStream* dataStream);

void DataStream_PushStr(DataStream* dataStream, Str value, Arena* arena);
Str DataStream_PopStr(DataStream* dataStream);

void DataStream_PushSliceI32(DataStream* dataStream, SliceI32 value, Arena* arena);
SliceI32 DataStream_PopSliceI32(DataStream* dataStream);

void DataStream_PushSliceStr(DataStream* dataStream, SliceStr value, Arena* arena);
SliceStr DataStream_PopSliceStr(DataStream* dataStream, Arena* arena);

#endif // WETMAN_UTILS_SERIALIZATION_H
