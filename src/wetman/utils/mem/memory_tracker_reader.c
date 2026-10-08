#include <wetman/utils/mem/memory_tracker_reader.h>

#include <wetman/utils/filesystem.h>
#include <wetman/utils/macro.h>

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>


typedef enum {
    ARENA_STATE_NEVER_SEEN,
    ARENA_STATE_ALIVE,
    ARENA_STATE_FREED,
} __ArenaState;


// Parses a single whitespace-trimmed dump line into `event`.
// Returns 1 on a matched event line, 0 for a blank line, -1 for a malformed
// line that matches no known event format.
static int __MemoryEvent_ParseLine(char* line, MemoryEvent* event)
{
    while (*line == ' ' || *line == '\t' || *line == '\r' || *line == '\n') {
        line++;
    }
    if (*line == '\0') {
        return 0;
    }

    long long         timestamp = 0;
    unsigned long long arenaId   = 0;
    unsigned long long allocSize = 0;
    int               consumed   = 0;

    int n = sscanf(line,
            "MemoryEvent { type: ARENA_CREATED, timestamp: %lld, arenaId: %llu }%n",
            &timestamp, &arenaId, &consumed);
    if (n == 2 && (usize)consumed == strlen(line)) {
        event->type      = MEMORY_EVENT_ARENA_CREATED;
        event->timestamp = (i64)timestamp;
        event->event.arenaCreated.arenaId = (usize)arenaId;
        return 1;
    }

    n = sscanf(line,
            "MemoryEvent { type: ARENA_FREED, timestamp: %lld, arenaId: %llu }%n",
            &timestamp, &arenaId, &consumed);
    if (n == 2 && (usize)consumed == strlen(line)) {
        event->type      = MEMORY_EVENT_ARENA_FREED;
        event->timestamp = (i64)timestamp;
        event->event.arenaFreed.arenaId = (usize)arenaId;
        return 1;
    }

    n = sscanf(line,
            "MemoryEvent { type: ARENA_RESET, timestamp: %lld, arenaId: %llu }%n",
            &timestamp, &arenaId, &consumed);
    if (n == 2 && (usize)consumed == strlen(line)) {
        event->type      = MEMORY_EVENT_ARENA_RESET;
        event->timestamp = (i64)timestamp;
        event->event.arenaReset.arenaId = (usize)arenaId;
        return 1;
    }

    n = sscanf(line,
            "MemoryEvent { type: MEMORY_ALLOCATED, timestamp: %lld, arenaId: %llu, allocSize: %llu }%n",
            &timestamp, &arenaId, &allocSize, &consumed);
    if (n == 3 && (usize)consumed == strlen(line)) {
        event->type      = MEMORY_EVENT_MEMORY_ALLOCATED;
        event->timestamp = (i64)timestamp;
        event->event.memoryAllocated.arenaId   = (usize)arenaId;
        event->event.memoryAllocated.allocSize = (usize)allocSize;
        return 1;
    }

    return -1;
}

// Walks the dump buffer line by line, trimming each line's trailing
// whitespace. Yields pointers to consecutive non-blank lines; returns NULL
// when exhausted. `it->lineCount` counts the yielded lines. When
// `nullTerminate` is true the trimmed line is NUL-terminated in place (safe
// only after the walking pass that still needs the '\n' delimiters is done).
typedef struct {
    char* cursor;
    usize lineCount;
} __LineCursor;

static char* __Lines_Next(__LineCursor* it, i32 nullTerminate)
{
    for (;;) {
        if (it->cursor == NULL || *it->cursor == '\0') {
            return NULL;
        }

        char* line = it->cursor;
        char* newline = strchr(line, '\n');
        char* end = newline ? newline : line + strlen(line);
        it->cursor = newline ? newline + 1 : NULL;

        while (end > line && (end[-1] == '\n' || end[-1] == '\r'
                || end[-1] == ' ' || end[-1] == '\t')) {
            end--;
        }
        if (nullTerminate) {
            *end = '\0';
        }

        if (end == line) {
            continue;
        }

        it->lineCount++;
        return line;
    }
}


i32 MemoryTrackerReader_ReadFromFile(Str path, Arena* arena, MemoryEventList* out)
{
    out->events = NULL;
    out->count  = 0;

    i32 fd = FS_OpenFile(path, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "MemoryTrackerReader: cannot open '%.*s'\n",
                (i32)path.len, path.data);
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) != 0 || st.st_size < 0) {
        close(fd);
        return -1;
    }
    const usize size = (usize)st.st_size;

    char* buf = Arena_Alloc(arena, size + 1);
    if (buf == NULL) {
        close(fd);
        return -1;
    }

    usize total = 0;
    while (total < size) {
        isize n = read(fd, buf + total, size - total);
        if (n > 0) {
            total += (usize)n;
            continue;
        }
        if (n == 0) {
            break;
        }
        if (errno == EINTR) {
            continue;
        }
        close(fd);
        return -1;
    }
    close(fd);
    buf[total] = '\0';

    // First pass: count the non-blank lines so the array is allocated once.
    // Must not terminate lines yet — the second pass still needs the '\n'
    // delimiters to walk the buffer again.
    __LineCursor it = { .cursor = buf, .lineCount = 0 };
    while (__Lines_Next(&it, FALSE) != NULL) {
    }
    const usize lineCount = it.lineCount;

    if (lineCount == 0) {
        return 0;
    }

    MemoryEvent* events = Arena_Alloc(arena, lineCount * sizeof(MemoryEvent));
    if (events == NULL) {
        return -1;
    }

    // Second pass: parse each captured line.
    MemoryEvent* head = events;
    it = (__LineCursor){ .cursor = buf, .lineCount = 0 };
    char* line;
    while ((line = __Lines_Next(&it, TRUE)) != NULL) {
        MemoryEvent event;
        const int status = __MemoryEvent_ParseLine(line, &event);
        if (status != 1) {
            fprintf(stderr, "MemoryTrackerReader: malformed event line: %s\n", line);
            return -1;
        }
        *head++ = event;
    }

    out->events = events;
    out->count  = lineCount;
    return 0;
}


i32 MemoryTrackerReader_VerifyAllArenasFreed(const MemoryEventList* list, Arena* arena)
{
    usize maxArenaId = 0;
    for (usize i = 0; i < list->count; i++) {
        const MemoryEvent* event = &list->events[i];
        const usize arenaId = (event->type == MEMORY_EVENT_MEMORY_ALLOCATED)
                ? event->event.memoryAllocated.arenaId
                : (event->type == MEMORY_EVENT_ARENA_CREATED)
                        ? event->event.arenaCreated.arenaId
                : (event->type == MEMORY_EVENT_ARENA_FREED)
                        ? event->event.arenaFreed.arenaId
                : (event->type == MEMORY_EVENT_ARENA_RESET)
                        ? event->event.arenaReset.arenaId
                        : 0;
        if (arenaId > maxArenaId) {
            maxArenaId = arenaId;
        }
    }

    __ArenaState* states = Arena_Alloc(arena, (maxArenaId + 1) * sizeof(__ArenaState));
    if (states == NULL) {
        return FALSE;
    }
    memset(states, 0, (maxArenaId + 1) * sizeof(__ArenaState)); // NEVER_SEEN

    i32 ok = TRUE;

    for (usize i = 0; i < list->count; i++) {
        const MemoryEvent* event = &list->events[i];

        switch (event->type) {
            case MEMORY_EVENT_ARENA_CREATED: {
                const usize arenaId = event->event.arenaCreated.arenaId;
                if (states[arenaId] != ARENA_STATE_NEVER_SEEN) {
                    fprintf(stderr,
                            "ARENA_CREATED duplicate for arena %zu at event %zu\n",
                            arenaId, i);
                    ok = FALSE;
                }
                states[arenaId] = ARENA_STATE_ALIVE;
                break;
            }
            case MEMORY_EVENT_ARENA_FREED: {
                const usize arenaId = event->event.arenaFreed.arenaId;
                if (states[arenaId] != ARENA_STATE_ALIVE) {
                    fprintf(stderr,
                            "ARENA_FREED for arena %zu at event %zu but the arena is not alive\n",
                            arenaId, i);
                    ok = FALSE;
                }
                states[arenaId] = ARENA_STATE_FREED;
                break;
            }
            case MEMORY_EVENT_ARENA_RESET: {
                const usize arenaId = event->event.arenaReset.arenaId;
                if (states[arenaId] != ARENA_STATE_ALIVE) {
                    fprintf(stderr,
                            "ARENA_RESET for arena %zu at event %zu but the arena is not alive\n",
                            arenaId, i);
                    ok = FALSE;
                }
                break;
            }
            case MEMORY_EVENT_MEMORY_ALLOCATED: {
                const usize arenaId = event->event.memoryAllocated.arenaId;
                if (states[arenaId] != ARENA_STATE_ALIVE) {
                    fprintf(stderr,
                            "MEMORY_ALLOCATED for arena %zu at event %zu but the arena is not alive\n",
                            arenaId, i);
                    ok = FALSE;
                }
                break;
            }
            case MEMORY_EVENT_UNDEFINED:
                break;
        }
    }

    // Arenas that were created but never freed are leaks.
    for (usize arenaId = 0; arenaId <= maxArenaId; arenaId++) {
        if (states[arenaId] == ARENA_STATE_ALIVE) {
            fprintf(stderr, "ARENA %zu was created but never freed\n", arenaId);
            ok = FALSE;
        }
    }

    return ok;
}
