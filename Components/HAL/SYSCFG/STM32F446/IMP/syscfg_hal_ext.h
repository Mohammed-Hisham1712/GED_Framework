#ifndef __SYSCFG_HAL_H__

#define __SYSCFG_HAL_H__

#include "syscfg_periph.h"
#include "assert.h"

#define SYSCFG_HAL_GET_HW(_hal)     ((_hal)->dev = SYSCFG_LL_GET_HW(0))


typedef struct 
{
    /* data */
    syscfg_dev_t dev;
} syscfg_hal_context_t;

void syscfg_hal_init(void);

void syscfg_hal_set_exti_port(syscfg_hal_context_t* hal, uint32_t line, uint32_t port);

#endif