#ifndef WETMAN_UTILS_MACRO_H
#define WETMAN_UTILS_MACRO_H

#ifdef __GNUC__

#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define MAYBE_UNUSED __attribute__((unused))

// TODO: define type safe in separate module
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

#define FALSE 0
#define TRUE 1

#else

#define UNLIKELY(x) (x)
#define LIKELY(x)   (x)
#define MAYBE_UNUSED

#endif

#endif // WETMAN_UTILS_MACRO_H 
