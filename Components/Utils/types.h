#ifndef _TYPES_H_

#define _TYPES_H_

#include <stdint.h>

#define WEAK            __attribute__((weak))
#define STATIC_INLINE   static inline


typedef uint8_t bool_t;

typedef enum
{
    OK = 0,
    FAILED = -1
} error_t;

#endif