#include <wetman/utils/mem/memory_tracker_reader.h>
#include <wetman/utils/test/macro.h>

#include <stdio.h>
#include <string.h>


static i32 WriteDump(const char* path, const char* content)
{
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        return -1;
    }
    fputs(content, file);
    fclose(file);
    return 0;
}

// Writes the dump to `path`, reads it back and runs the verifier.
// Returns TRUE if it parsed and verified, FALSE if it parsed but verification
// failed, and -1 if reading or parsing failed.
static i32 ReadAndVerify(const char* path, const char* content)
{
    if (WriteDump(path, content) != 0) {
        return -1;
    }

    Arena arena = Arena_New();
    MemoryEventList list;
    const i32 readResult =
            MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list);

    i32 result;
    if (readResult != 0) {
        result = -1;
    } else {
        result = MemoryTrackerReader_VerifyAllArenasFreed(&list, &arena);
    }

    Arena_Free(&arena);
    return result;
}


TEST(MemoryTrackerReaderTest_ReadValidDump)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    char path[512];
    snprintf(path, sizeof(path), "%s/dump.txt", dir);

    const char* dump =
        "MemoryEvent { type: ARENA_CREATED, timestamp: 100, arenaId: 0 }\n"
        "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: 101, arenaId: 0, allocSize: 64 }\n"
        "MemoryEvent { type: ARENA_RESET, timestamp: 102, arenaId: 0 }\n"
        "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: 103, arenaId: 0, allocSize: 128 }\n"
        "MemoryEvent { type: ARENA_CREATED, timestamp: 104, arenaId: 1 }\n"
        "MemoryEvent { type: ARENA_FREED, timestamp: 105, arenaId: 1 }\n"
        "MemoryEvent { type: ARENA_FREED, timestamp: 106, arenaId: 0 }\n";
    ASSERT_EQ(WriteDump(path, dump), 0);

    Arena arena = Arena_New();
    MemoryEventList list;
    ASSERT_EQ(MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list), 0);
    ASSERT_EQ(list.count, 7);

    EXPECT_EQ(list.events[0].type, MEMORY_EVENT_ARENA_CREATED);
    EXPECT_EQ(list.events[0].timestamp, 100);
    EXPECT_EQ(list.events[0].event.arenaCreated.arenaId, 0);

    EXPECT_EQ(list.events[1].type, MEMORY_EVENT_MEMORY_ALLOCATED);
    EXPECT_EQ(list.events[1].event.memoryAllocated.arenaId, 0);
    EXPECT_EQ(list.events[1].event.memoryAllocated.allocSize, 64);

    EXPECT_EQ(list.events[2].type, MEMORY_EVENT_ARENA_RESET);
    EXPECT_EQ(list.events[2].event.arenaReset.arenaId, 0);

    EXPECT_EQ(list.events[4].type, MEMORY_EVENT_ARENA_CREATED);
    EXPECT_EQ(list.events[4].event.arenaCreated.arenaId, 1);

    EXPECT_EQ(list.events[5].type, MEMORY_EVENT_ARENA_FREED);
    EXPECT_EQ(list.events[5].event.arenaFreed.arenaId, 1);

    EXPECT_EQ(list.events[6].type, MEMORY_EVENT_ARENA_FREED);
    EXPECT_EQ(list.events[6].event.arenaFreed.arenaId, 0);

    EXPECT_EQ(MemoryTrackerReader_VerifyAllArenasFreed(&list, &arena), TRUE);

    Arena_Free(&arena);
}

TEST(MemoryTrackerReaderTest_ReadErrors)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    // A line that matches no known event format -> -1
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/malformed.txt", dir);
        ASSERT_EQ(WriteDump(path,
                "MemoryEvent { type: ARENA_CREATED, timestamp: 100, arenaId: 0 }\n"
                "garbage line\n"), 0);

        Arena arena = Arena_New();
        MemoryEventList list;
        EXPECT_EQ(MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list), -1);
        Arena_Free(&arena);
    }

    // Trailing junk after an otherwise valid line -> -1
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/junk.txt", dir);
        ASSERT_EQ(WriteDump(path,
                "MemoryEvent { type: ARENA_FREED, timestamp: 100, arenaId: 0 } extra\n"), 0);

        Arena arena = Arena_New();
        MemoryEventList list;
        EXPECT_EQ(MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list), -1);
        Arena_Free(&arena);
    }

    // Missing file -> -1
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/does_not_exist.txt", dir);

        Arena arena = Arena_New();
        MemoryEventList list;
        EXPECT_EQ(MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list), -1);
        Arena_Free(&arena);
    }

    // A dump with only blank lines -> 0 events, verification trivially passes
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/blank.txt", dir);
        ASSERT_EQ(WriteDump(path, "\n\n  \n\t\n"), 0);

        Arena arena = Arena_New();
        MemoryEventList list;
        ASSERT_EQ(MemoryTrackerReader_ReadFromFile(Str_FromCStr(path), &arena, &list), 0);
        EXPECT_EQ(list.count, 0);
        EXPECT_EQ(MemoryTrackerReader_VerifyAllArenasFreed(&list, &arena), TRUE);
        Arena_Free(&arena);
    }
}

TEST(MemoryTrackerReaderTest_VerifyConfirmsCleanDump)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    char path[512];
    snprintf(path, sizeof(path), "%s/clean.txt", dir);

    const char* dump =
        "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
        "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: 2, arenaId: 0, allocSize: 32 }\n"
        "MemoryEvent { type: ARENA_RESET, timestamp: 3, arenaId: 0 }\n"
        "MemoryEvent { type: ARENA_CREATED, timestamp: 4, arenaId: 1 }\n"
        "MemoryEvent { type: ARENA_FREED, timestamp: 5, arenaId: 1 }\n"
        "MemoryEvent { type: ARENA_FREED, timestamp: 6, arenaId: 0 }\n";
    EXPECT_EQ(ReadAndVerify(path, dump), TRUE);
}

TEST(MemoryTrackerReaderTest_VerifyDetectsLeak)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    char path[512];
    snprintf(path, sizeof(path), "%s/leak.txt", dir);

    // Arena 0 is created but never freed.
    const char* dump =
        "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
        "MemoryEvent { type: ARENA_CREATED, timestamp: 2, arenaId: 1 }\n"
        "MemoryEvent { type: ARENA_FREED, timestamp: 3, arenaId: 1 }\n";
    EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
}

TEST(MemoryTrackerReaderTest_VerifyDetectsInvalidTransitions)
{
    char dir[256];
    CREATE_TMP_DIR(dir);

    // Double free
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/double_free.txt", dir);
        const char* dump =
            "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
            "MemoryEvent { type: ARENA_FREED, timestamp: 2, arenaId: 0 }\n"
            "MemoryEvent { type: ARENA_FREED, timestamp: 3, arenaId: 0 }\n";
        EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
    }

    // Free without a create
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/free_without_create.txt", dir);
        const char* dump =
            "MemoryEvent { type: ARENA_FREED, timestamp: 1, arenaId: 0 }\n";
        EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
    }

    // Allocation on an arena that was never created
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/alloc_unknown.txt", dir);
        const char* dump =
            "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
            "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: 2, arenaId: 5, allocSize: 16 }\n";
        EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
    }

    // Allocation after the arena was freed
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/alloc_after_free.txt", dir);
        const char* dump =
            "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
            "MemoryEvent { type: ARENA_FREED, timestamp: 2, arenaId: 0 }\n"
            "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: 3, arenaId: 0, allocSize: 16 }\n";
        EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
    }

    // Duplicate create for the same arena id
    {
        char path[512];
        snprintf(path, sizeof(path), "%s/duplicate_create.txt", dir);
        const char* dump =
            "MemoryEvent { type: ARENA_CREATED, timestamp: 1, arenaId: 0 }\n"
            "MemoryEvent { type: ARENA_FREED, timestamp: 2, arenaId: 0 }\n"
            "MemoryEvent { type: ARENA_CREATED, timestamp: 3, arenaId: 0 }\n";
        EXPECT_EQ(ReadAndVerify(path, dump), FALSE);
    }
}
