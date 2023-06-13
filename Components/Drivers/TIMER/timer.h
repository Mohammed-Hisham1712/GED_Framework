#ifndef __TIMER_H__

#define __TIMER_H__

#include "types.h"

#include <stdint.h>

typedef struct
{
    uint8_t     started;
    uint64_t    start_milliseconds;
} ms_timer_t;

/**
 * @brief 
 * 
 * @return error_t 
 */
error_t timer_time_base_init(void);

/**
 * @brief 
 * 
 * @param p_timer 
 * @return error_t 
 */
error_t timer_clear(ms_timer_t* p_timer);

/**
 * @brief 
 * 
 * @param p_timer 
 * @return error_t 
 */
error_t timer_start(ms_timer_t* p_timer);

/**
 * @brief 
 * 
 * @param p_timer 
 * @return uint64_t
 */
uint64_t timer_elapsed(const ms_timer_t* p_timer);

/**
 * @brief 
 * 
 * @param p_timer 
 * @return uint8_t 
 */
uint8_t timer_is_started(const ms_timer_t* p_timer);

/**
 * @brief 
 * 
 * @return uint64_t 
 */
uint64_t timer_get_milliseconds(void);

#endif