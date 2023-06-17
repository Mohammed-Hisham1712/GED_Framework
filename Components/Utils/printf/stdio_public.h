#ifndef __STDIO_PUBLIC_H__

#define __STDIO_PUBLIC_H__

#include "printf.h"

#define EOF     (-1)


/**
 * @brief An implementation of the C standard's putchar
 * 
 * @note in libc's putchar, the parameter type is an int; this was intended to support the
 * representation of either a proper character or EOF in a variable - but this is really not
 * meaningful to pass into putchar and is discouraged today.
 * 
 * @param ch The single character to print
 * @return Return the character written as an unsigned char cast to an int or -1 on error
 */

PRINTF_VISIBILITY
int putch_(char ch);


/**
 * @brief An implementation of the C standard's puts
 * 
 * @param s writes the string s and a trailing new line to stdout
 * @return return a non-negative number on success, or -1 on error
 */

PRINTF_VISIBILITY
int puts_(const char* s);

#if PRINTF_ALIAS_STANDARD_FUNCTION_NAMES_SOFT
#define putchar     putch_
#define puts        puts_
#endif

#endif