#include <stdlib.h>
#include "rand_uint64.h"

#if RAND_MAX / 256 >= 0xFFFFFFFFFFFFFF
#define LOOP_COUNT 1
#elif RAND_MAX / 256 >= 0xFFFFFF
#define LOOP_COUNT 2
#elif RAND_MAX / 256 >= 0x3FFFF
#define LOOP_COUNT 3
#elif RAND_MAX / 256 >= 0x1FF
#define LOOP_COUNT 4
#else
#define LOOP_COUNT 5
#endif

/* 64-bit random integer — source: https://stackoverflow.com/a/33021408 */
uint64_t rand_uint64(void)
{
    uint64_t r = 0;
    for (int i = LOOP_COUNT; i > 0; i--)
    {
        r = r * (RAND_MAX + (uint64_t)1) + rand();
    }
    return r;
}
