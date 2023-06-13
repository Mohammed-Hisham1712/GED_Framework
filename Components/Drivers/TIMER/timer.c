#include "timer_private.h"
#include "timer.h"

#include "TIM_BASIC_hal.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

static struct ms_timer_ctrl_t timer_ctrl;

void timer_one_millisecond_passed(tim_basic_hal_context_t* p_timer)
{
    (void) p_timer;

    ++timer_ctrl.milliseconds_elapsed;
}

error_t timer_time_base_init(void)
{
    error_t l_ret;
    tim_basic_config_t tim_config;

    /* Calculations based on 16MhHz internal clock */
    tim_config.prescaler = 1000;
    tim_config.reload = 15;
    tim_config.callback = &timer_one_millisecond_passed;

    l_ret = FAILED;

    memset(&timer_ctrl, 0, sizeof(struct ms_timer_ctrl_t));

    timer_ctrl.hal_timer = tim_basic_hal_init(TIM_BASIC_TIM7, &tim_config);
    if(timer_ctrl.hal_timer)
    {
        tim_basic_hal_start(timer_ctrl.hal_timer);
        l_ret = OK;
    }

    return l_ret;
}

uint64_t timer_get_milliseconds(void)
{
    return timer_ctrl.milliseconds_elapsed;
}

error_t timer_clear(ms_timer_t* p_timer)
{
    if(p_timer)
    {
        memset(p_timer, 0, sizeof(ms_timer_t));

        return OK;
    }

    return FAILED;
}

error_t timer_start(ms_timer_t* p_timer)
{
    if(p_timer)
    {
        p_timer->started = 1;
        p_timer->start_milliseconds = timer_get_milliseconds();

        return OK;
    }

    return FAILED;
}

uint64_t timer_elapsed(const ms_timer_t* p_timer)
{
    uint64_t elapsed = 0;

    if(p_timer && p_timer->started)
    {
        elapsed = timer_get_milliseconds() - p_timer->start_milliseconds;
    }

    return elapsed;
}

uint8_t timer_is_started(const ms_timer_t* p_timer)
{
    if(p_timer)
    {
        return p_timer->started;
    }

    return 0;
}