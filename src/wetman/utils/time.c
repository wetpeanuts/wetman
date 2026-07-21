#include <wetman/utils/time.h>

#include <time.h>


long long currentTimestampMs(void)
{
    struct timespec timeSpec;
    clock_gettime(CLOCK_MONOTONIC, &timeSpec);

    return (long long)timeSpec.tv_sec * 1000 + timeSpec.tv_nsec / 1000000;
}
