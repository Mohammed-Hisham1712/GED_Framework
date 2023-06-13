#ifndef __TIM_BASIC_TYPES_H__

#define __TIM_BASIC_TYPES_H__

#include "TIM_BASIC_periph.h"

#include <stdint.h>

typedef struct TIM_BASIC_HAL_CONTEXT tim_basic_hal_context_t;

typedef void (*tim_basic_callback_t) (tim_basic_hal_context_t*);

enum
{
    TIM_BASIC_TIM6 = 6,
    TIM_BASIC_TIM7 = 7,
};

typedef struct
{
    uint16_t prescaler;
    uint16_t reload;
    tim_basic_callback_t callback;
} tim_basic_config_t;

struct TIM_BASIC_HAL_CONTEXT
{
    tim_basic_dev_t     dev;
    tim_basic_callback_t callback;    
};

#endif