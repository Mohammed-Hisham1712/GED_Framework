#include "assert.h"

#include <string.h>

assert_info_t assert_info;

void __assert(const char* filename, uint32_t line)
{
    strncpy(assert_info.file, filename, sizeof(assert_info.file));
    assert_info.line = line;

    __asm("bkpt 1");
}