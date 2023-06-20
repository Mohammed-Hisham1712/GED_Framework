#ifndef __LOG_H__

#define __LOG_H__

#include <stdint.h>

#define LOG_LEVEL_NONE      0
#define LOG_LEVEL_ERROR     1
#define LOG_LEVEL_WARNING   2
#define LOG_LEVEL_INFO      3
#define LOG_LEVEL_DEBUG     4
#define LOG_LEVEL_MAX       5

typedef uint8_t log_level_t;
typedef int (*putchar_like_t) (char);



/**
 * @brief Set function used to output log messages. By default log output is directed to
 * stdout through C standard's putchar like function putch_. Use this function to redirect log
 * messages to other destinations such as Network or log files. 
 * 
 * @param new_func new function to be used for output. Must have signature as putchar.
 * @return putchar_like_t Old function used for output in case you want to set it back.
 */
putchar_like_t log_set_putchar(putchar_like_t new_func);


/**
 * @brief Write a log message with specific log level and tag
 * 
 * @param level log level of the log message.
 * 
 * @param tag   A string appended in front of a log message to identify the source file or
 *              component that wrote this log message 
 * 
 * @param format A format string similar to the one used with C standard's printf functions
 * 
 * @param va_list Variable args that matches the number of specifier in the string format
 */
void log_write(log_level_t level, const char* tag, const char* format, ...)
                                                        __attribute__((format(printf, 3, 4)));

/**
 * @brief 
 * 
 * @param level 
 * @param tag 
 * @param buffer 
 * @param size 
 */
void log_hexdump(log_level_t level, const char* tag, const void* buffer, uint16_t size);


#ifndef LOG_LOCAL_LEVEL
#define LOG_LOCAL_LEVEL     CONFIG_LOG_DEFAULT_LEVEL
#endif

#define LOG_LOCAL(level, tag, fmt, ...)     log_write(level, tag, fmt, ##__VA_ARGS__)

#if (LOG_LOCAL_LEVEL >= LOG_LEVEL_ERROR)
#define LOGE(tag, fmt, ...)                 LOG_LOCAL(LOG_LEVEL_ERROR, tag, fmt, ##__VA_ARGS__)
#else
#define LOGE(tag, fmt, ...)
#endif

#if (LOG_LOCAL_LEVEL >= LOG_LEVEL_WARNING)
#define LOGW(tag, fmt, ...)                 LOG_LOCAL(LOG_LEVEL_WARNING, tag, fmt, ##__VA_ARGS__)
#else
#define LOGW(tag, fmt, ...)
#endif

#if (LOG_LOCAL_LEVEL >= LOG_LEVEL_INFO)
#define LOGI(tag, fmt, ...)                 LOG_LOCAL(LOG_LEVEL_INFO, tag, fmt, ##__VA_ARGS__)
#else
#define LOGI(tag, fmt, ...)
#endif

#if (LOG_LOCAL_LEVEL >= LOG_LEVEL_DEBUG)
#define LOGD(tag, fmt, ...)                 LOG_LOCAL(LOG_LEVEL_DEBUG, tag, fmt, ##__VA_ARGS__)
#else
#define LOGD(tag, fmt, ...)
#endif

#endif