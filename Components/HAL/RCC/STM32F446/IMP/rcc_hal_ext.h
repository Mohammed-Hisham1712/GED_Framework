#ifndef _RCC_HAL_H_

#define _RCC_HAL_H_

#include "rcc_ll.h"

#define RCC_HAL_AHB1_GPIO(_port)    (_port)

typedef enum
{
    RCC = 0
} rcc_t;

typedef enum
{
    RCC_HAL_GPIOA,
    RCC_HAL_GPIOB,
    RCC_HAL_GPIOC,
    RCC_HAL_GPIOD,
    RCC_HAL_GPIOE,
    RCC_HAL_GPIOF,
    RCC_HAL_GPIOG,
    RCC_HAL_GPIOH,
    RCC_HAL_DMA1 = 21,
    RCC_HAL_DMA2 = 22,
}rcc_hal_ahb1_periph_t;

typedef enum
{
    RCC_HAL_USART1 = 4,
    RCC_HAL_USART6 = 5,
    RCC_HAL_SYSCFG = 14,
}rcc_hal_apb2_periph_t;

typedef struct 
{
    rcc_dev_t dev;
}rcc_hal_context_t;

#define RCC_HAL_GET_HW(hal, num)            ((hal)->dev = RCC_LL_GET_HW(num))

#define rcc_hal_ahb1_en_clk(hal, periph)    rcc_ll_ahb1_en_clk((hal)->dev, periph)

#define rcc_hal_apb2_en_clk(hal, periph)    rcc_ll_apb2_en_clk((hal)->dev, periph)

#endif