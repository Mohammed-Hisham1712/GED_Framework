#ifndef __TIMER_PRIVATE_H__

#define __TIMER_PRIVATE_H__

#include "TIM_BASIC_hal.h"

#include <stdint.h>

struct ms_timer_ctrl_t
{
    uint64_t    milliseconds_elapsed;
    tim_basic_hal_context_t*    hal_timer;
};

#endif