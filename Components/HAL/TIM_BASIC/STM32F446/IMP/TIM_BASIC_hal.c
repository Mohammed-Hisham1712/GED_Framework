#include "TIM_BASIC_hal.h"
#include "TIM_BASIC_types.h"
#include "TIM_BASIC_ll.h"

#include "rcc_hal_ext.h"
#include "stm32f446xx.h"
#include "types.h"

#include <stdint.h>
#include <stddef.h>

#define TIM_BASIC_HAL_IS_TIMER(tim) \
            ((((tim) == TIM_BASIC_TIM6) || ((tim) == TIM_BASIC_TIM7)) ? 1 : 0)

static tim_basic_hal_context_t basic_timers[2];


static void tim_basic_hal_irq_handler(tim_basic_hal_context_t* p_timer)
{
    if(p_timer)
    {
        TIM_BASIC_LL_CLEAR_IRQ_FLAG(p_timer->dev);

        if(p_timer->callback)
        {
            p_timer->callback(p_timer);
        }
    }
}

void tim6_irq_handler(void)
{
    tim_basic_hal_irq_handler(&basic_timers[0]);
}

void tim7_irq_handler(void)
{
    tim_basic_hal_irq_handler(&basic_timers[1]);
}

tim_basic_hal_context_t* tim_basic_hal_init(uint8_t tim, tim_basic_config_t* config)
{
    rcc_hal_context_t rcc;
    tim_basic_hal_context_t* p_timer;

    if(!TIM_BASIC_HAL_IS_TIMER(tim))
    {
        return NULL;
    }

    if(!config)
    {
        return NULL;
    }

    RCC_HAL_GET_HW(&rcc, RCC);

    if(tim == TIM_BASIC_TIM6)
    {
        p_timer = &basic_timers[0];
        TIM_BASIC_HAL_GET_HW(p_timer, 0);
        rcc_hal_apb1_en_clk(&rcc, RCC_HAL_TIM6);
        NVIC_EnableIRQ(TIM6_DAC_IRQn);
    }
    else
    {
        p_timer = &basic_timers[1];
        TIM_BASIC_HAL_GET_HW(p_timer, 1);
        rcc_hal_apb1_en_clk(&rcc, RCC_HAL_TIM7);
        NVIC_EnableIRQ(TIM7_IRQn);
    }

    p_timer->callback = config->callback;

    TIM_BASIC_LL_DISABLE_COUNTER(p_timer->dev);
    TIM_BASIC_LL_ENABLE_UEV(p_timer->dev);
    TIM_BASIC_LL_ENABLE_ARR_PRELOAD(p_timer->dev);
    TIM_BASIC_LL_CLEAR_IRQ_FLAG(p_timer->dev);
    TIM_BASIC_LL_ENABLE_IRQ(p_timer->dev);
    TIM_BASIC_LL_DISABLE_DMA_REQ(p_timer->dev);

    TIM_BASIC_LL_SET_COUNT(p_timer->dev, 0);
    TIM_BASIC_LL_SET_PRESCALER(p_timer->dev, config->prescaler);
    TIM_BASIC_LL_SET_ARR(p_timer->dev, config->reload);

    return p_timer;
}

error_t tim_basic_hal_start(tim_basic_hal_context_t* p_timer)
{
    if(!p_timer)
    {
        return FAILED;
    }

    TIM_BASIC_LL_ENABLE_COUNTER(p_timer->dev);

    return OK;
}

error_t tim_basic_hal_stop(tim_basic_hal_context_t* p_timer)
{
    if(!p_timer)
    {
        return FAILED;
    }

    TIM_BASIC_LL_DISABLE_COUNTER(p_timer->dev);

    return OK;
}