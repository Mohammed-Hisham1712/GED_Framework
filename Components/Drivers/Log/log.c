#include "log.h"

#include "stdio_public.h"
#include "timer.h"
#include "types.h"

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <string.h>


#if CONFIG_LOG_COLORS

#define LOG_COLOR_END       "\033[0m"

const char* colors[LOG_LEVEL_MAX] = 
{
    "\033[0;0m",    // None
    "\033[0;31m",   // Red
    "\033[0;35m",   // Magenta
    "\033[0;33m",   // Yellow
    "\033[0;32m",   // Green
};

#endif  /* CONFIG_LOG_COLORS */

putchar_like_t putchar_like_func = &putch_;


const char log_prefixes[LOG_LEVEL_MAX] = 
{
    'N',
    'E',
    'W',
    'I',
    'D',
};

static error_t log_write_str(const char* str)
{
    if(!str || !putchar_like_func)
    {
        return FAILED;
    }

    while(*str)
    {
        if(putchar_like_func(*str++) == EOF)
        {
            return FAILED;
        }
    }

    return OK;
}

static uint64_t log_get_timestamp(void)
{
    return timer_get_milliseconds();
}

putchar_like_t log_set_putchar(putchar_like_t new_func)
{
    putchar_like_t old_func;

    old_func = NULL;

    if(new_func)
    {
        old_func = putchar_like_func;
        putchar_like_func = new_func;
    }

    return old_func;
}

void log_write(log_level_t level, const char* tag, const char* format, ...)
{
    va_list args;
    uint64_t timestamp;
    char log_buf[104];
    int ret;
    char prefix;

    if(!tag || !format)
    {
        return;
    }

    #if CONFIG_LOG_COLORS
    const char* color = (level < LOG_LEVEL_MAX) ? 
                                    colors[level] : colors[LOG_LEVEL_NONE];
    log_write_str(color);
    #endif /* CONFIG_LOG_COLORS */

    prefix = (level < LOG_LEVEL_MAX) ? 
                        log_prefixes[level] : log_prefixes[LOG_LEVEL_NONE];

    timestamp = log_get_timestamp();
    
    snprintf_(log_buf, sizeof(log_buf), "%c (%llu) %s: ", prefix, timestamp, tag);

    if(log_write_str(log_buf) != OK)
    {
        return;
    }

    va_start(args, format);
    ret = vsnprintf_(log_buf, sizeof(log_buf), format, args);
    va_end(args);

    if(ret >= sizeof(log_buf))
    {
        return;
    }

    log_write_str(log_buf);
    log_write_str("\r\n");

    #if CONFIG_LOG_COLORS
    log_write_str(LOG_COLOR_END);
    #endif /* CONFIG_LOG_COLORS */
}

#if 0
/*
* TODO: Complete this function
*/
void log_hexdump(log_level_t level, const char* tag, const void* buffer, uint16_t size)
{
    const uint8_t*psrc;
    char* pdest;
    uint16_t remaining;
    uint8_t to_copy;
    uint8_t indx;
    uint8_t is_msn;
    uint32_t addr;
    char str_hexaddr[11];
    char str_mhexdump[25];
    char str_shexdump[25];
    char str_log[104];

    if(!tag || !buffer || (size == 0))
    {
        return;
    }

    addr = (uint32_t) buffer;
    psrc = (const uint8_t*) buffer;
    pdest = &str_shexdump[0];

    remaining = size;
    is_msn = 0;

    while(remaining)
    {
        to_copy = (remaining > 8) ? 8 : remaining;

        for(indx = 0; indx < to_copy; indx++)
        {
            snprintf_(pdest, 4, "%02x ", *psrc);
            
            ++psrc;
            pdest += 3;
        }

        if(to_copy < 8)
        {
            memset(pdest, ' ', (8 - to_copy) * 3);
            pdest += (8 - to_copy) * 3;
        }

        if(!is_msn)
        {
            is_msn = 1;
            *pdest = '\0';
            pdest = &str_mhexdump[0];
        }
        else
        {
            is_msn = 0;
            *pdest = '\0';
            pdest = &str_shexdump[0];
            snprintf_(str_hexaddr, sizeof(str_hexaddr), "0x%08lx", addr);
            snprintf_(str_log, sizeof(str_log), "%s\t%s\t%s", 
                                str_hexaddr, str_shexdump, str_mhexdump);
            log_write_str(str_log);
            log_write_str("\r\n");
        }

        remaining -= to_copy;
        addr += to_copy;
    }
}

#endif