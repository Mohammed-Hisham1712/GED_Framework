#ifndef __TIM_BASIC_PERIPH_H__

#define __TIM_BASIC_PERIPH_H__

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

typedef volatile struct TIM_BASIC_PERIPH* tim_basic_dev_t;

#define _TIM6   ((tim_basic_dev_t) 0x40001000UL)
#define _TIM7   ((tim_basic_dev_t) 0x40001400UL)

#define TIM_BASIC_CR1_CEN_S     BIT(0)
#define TIM_BASIC_CR1_UDIS_S    BIT(1)
#define TIM_BASIC_CR1_URS_S     BIT(2)
#define TIM_BASIC_CR1_OPM_S     BIT(3)
#define TIM_BASIC_CR1_ARPE_S    BIT(7)

#define TIM_BASIC_CR2_MMS_S     (4)
#define TIM_BASIC_CR2_MMS_M     (0x7)

#define TIM_BASIC_DIER_UIE_S    BIT(0)
#define TIM_BASIC_DIER_UDE_S    BIT(8)

#define TIM_BASIC_SR_UIF_S      BIT(0)

#define TIM_BASIC_EGR_UG_S      BIT(0)


struct TIM_BASIC_PERIPH
{
    uint32_t cr1;
    uint32_t cr2;
    uint32_t reserved_1;
    uint32_t dier;
    uint32_t sr;
    uint32_t egr;
    uint32_t reserved[3];
    uint32_t cnt;
    uint32_t psc;
    uint32_t arr;
};

static const tim_basic_dev_t TIM_BASIC_LL_TIMx[] = 
{
    _TIM6,
    _TIM7
};

#define TIM_BASIC_LL_DEVS   (sizeof(TIM_BASIC_LL_TIMx) / sizeof(TIM_BASIC_LL_TIMx[0]))

#define TIM_BASIC_LL_GET_HW(num) \
                        (((num) < TIM_BASIC_LL_DEVS) ? TIM_BASIC_LL_TIMx[num] : NULL)

#endif