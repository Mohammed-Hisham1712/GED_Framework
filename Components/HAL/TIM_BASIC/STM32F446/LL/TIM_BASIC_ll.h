#ifndef __TIM_BASIC_LL_H__

#define __TIM_BASIC_LL_H__

#include "TIM_BASIC_periph.h"

#include "bit_math.h"
#include "types.h"


#define TIM_BASIC_LL_ENABLE_COUNTER(hw)     REG_SET_BIT((hw)->cr1, TIM_BASIC_CR1_CEN_S)
#define TIM_BASIC_LL_DISABLE_COUNTER(hw)    REG_CLR_BIT((hw)->cr1, TIM_BASIC_CR1_CEN_S)
#define TIM_BASIC_LL_IS_COUNTER_ENABLED(hw) tim_basic_ll_is_counter_enabled(hw)

#define TIM_BASIC_LL_ENABLE_UEV(hw)         REG_CLR_BIT((hw)->cr1, TIM_BASIC_CR1_UDIS_S)
#define TIM_BASIC_LL_DISABLE_UEV(hw)        REG_SET_BIT((hw)->cr1, TIM_BASIC_CR1_UDIS_S)
#define TIM_BASIC_LL_IS_UEV_ENABLED(hw)     tim_basic_ll_is_uev_enabled(hw)

#define TIM_BASIC_LL_ENABLE_ARR_PRELOAD(hw) \
                                            REG_SET_BIT((hw)->cr1, TIM_BASIC_CR1_ARPE_S)
#define TIM_BASIC_LL_DISABLE_ARR_PRELOAD(hw) \
                                            REG_CLR_BIT((hw)->cr1, TIM_BASIC_CR1_ARPE_S)
#define TIM_BASIC_LL_IS_PRELOAD_ENABLED(hw) \
                                            tim_basic_ll_is_preload_enabled(hw)

#define TIM_BASIC_LL_ENABLE_IRQ(hw)         REG_SET_BIT((hw)->dier, TIM_BASIC_DIER_UIE_S)
#define TIM_BASIC_LL_DISABLE_IRQ(hw)        REG_CLR_BIT((hw)->dier, TIM_BASIC_DIER_UIE_S)

#define TIM_BASIC_LL_ENABLE_DMA_REQ(hw)     REG_SET_BIT((hw)->dier, TIM_BASIC_DIER_UDE_S)
#define TIM_BASIC_LL_DISABLE_DMA_REQ(hw)    REG_CLR_BIT((hw)->dier, TIM_BASIC_DIER_UDE_S)

#define TIM_BASIC_LL_CLEAR_IRQ_FLAG(hw)     REG_CLR_BIT((hw)->sr, TIM_BASIC_SR_UIF_S)
#define TIM_BASIC_LL_GET_IRQ_FLAG(hw)       REG_GET_BIT((hw)->sr, TIM_BASIC_SR_UIF_S)

#define TIM_BASIC_LL_SW_REINIT(hw)          REG_SET_BIT((hw)->egr, TIM_BASIC_EGR_UG_S)

#define TIM_BASIC_LL_SET_COUNT(hw, count)   ((hw)->cnt = ((count) & 0xFFFF))
#define TIM_BASIC_LL_GET_COUNT(hw)          (((hw)->cnt) & 0xFFFF)

#define TIM_BASIC_LL_SET_PRESCALER(hw, pre) ((hw)->psc = ((pre) & 0xFFFF))
#define TIM_BASIC_LL_GET_PRESCALER(hw)      (((hw)->psc) & 0xFFFF)

#define TIM_BASIC_LL_SET_ARR(hw, reload)    ((hw)->arr = ((reload) & 0xFFFF))
#define TIM_BASIC_LL_GET_ARR(hw)            (((hw)->arr) & 0xFFFF)



STATIC_INLINE uint8_t tim_basic_ll_is_counter_enabled(tim_basic_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, TIM_BASIC_CR1_CEN_S) ? 1 : 0;
}

STATIC_INLINE uint8_t tim_basic_ll_is_uev_enabled(tim_basic_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, TIM_BASIC_CR1_UDIS_S) ? 1 : 0;
}

STATIC_INLINE uint8_t tim_basic_ll_is_preload_enabled(tim_basic_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, TIM_BASIC_CR1_ARPE_S) ? 1 : 0;
}

#endif