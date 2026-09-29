#include <wetman/utils/time.h>

#include <time.h>


static long long __timestampMs(clockid_t clockId)
{
    struct timespec timeSpec;
    clock_gettime(clockId, &timeSpec);

    return (long long)timeSpec.tv_sec * 1000 + timeSpec.tv_nsec / 1000000;
}

static long long __timestampNs(clockid_t clockId)
{
    struct timespec timeSpec;
    clock_gettime(clockId, &timeSpec);

    return (long long)timeSpec.tv_sec * 1000000000 + timeSpec.tv_nsec;
}

// Monotonic time since boot, for measuring durations.
long long currentTimestampMs(void)
{
    return __timestampMs(CLOCK_MONOTONIC);
}

// Unix epoch time in milliseconds, for wall clock timestamps.
long long currentUnixTimestampMs(void)
{
    return __timestampMs(CLOCK_REALTIME);
}

// Unix epoch time in nanoseconds, for high resolution wall clock timestamps.
long long currentUnixTimestampNs(void)
{
    return __timestampNs(CLOCK_REALTIME);
}
