#ifndef _ASSERT_H_

#define _ASSERT_H_

#include <stdint.h>

/**
 * @brief Structure to hold assertion information
 * pc: Program Counter
 * lr: Link Register
 * line: line number where assertion is called
 * file: File name where assertion is called
 */
typedef struct 
{
    uint32_t pc;
    uint32_t lr;

    uint32_t line;
    char file[256];
} assert_info_t;

void __assert(const char* filename, uint32_t line);

/**
 * @brief assertion macro. Execute if exp is false
 * @see https://interrupt.memfault.com/blog/asserts-in-embedded-systems
 * 
 * @example
 * ASSERT(buff != NULL);
 */
#define ASSERT(exp) \
    do  \
    { \
        if(!(exp))  \
        {   \
            __assert(__FILE__, __LINE__);  \
        }   \
    } while(0)

#endif