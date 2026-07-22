#ifndef WETMAN_UTILS_MACRO_H
#define WETMAN_UTILS_MACRO_H

#ifdef __GNUC__

#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#define LIKELY(x)   __builtin_expect(!!(x), 1)
#define MAYBE_UNUSED __attribute__((unused))

#else

#define UNLIKELY(x) (x)
#define LIKELY(x)   (x)
#define MAYBE_UNUSED

#endif

#endif // WETMAN_UTILS_MACRO_H 
