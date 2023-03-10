#ifndef _GPIO_PERIPH_H__
#define _GPIO_PERIPH_H__

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

#define GPIO_LL_PINS        16UL

/* GPIO_MODER */
#define GPIO_MODE_S(_pin)   ((_pin) << 1UL)
#define GPIO_MODE_M         0x3

/* GPIO_OSPEEDR */
#define GPIO_SPEED_S(_pin)  ((_pin) << 1UL)
#define GPIO_SPEED_M        0x3

/* GPIO_PUPDR */
#define GPIO_PUPD_S(_pin)   ((_pin) << 1UL)
#define GPIO_PUPD_M         0x3

/* GPIO_BSRR */
#define GPIO_BSR_RESET_S(_pin)  (BIT(_pin) << GPIO_LL_PINS)
#define GPIO_BSR_RESET_M(_mask)   ((_mask) << GPIO_LL_PINS)

#define GPIO_AFR_S(pin)     ((pin) << 2UL)
#define GPIO_AFR_M          (0x0F)     

/* GPIO peripheral registers */
typedef struct {
    uint32_t moder;
    uint32_t otyper;
    uint32_t ospeedr;
    uint32_t pupdr;
    uint32_t idr;
    uint32_t odr;
    uint32_t bsrr;
    uint32_t lckr;
    uint32_t afrl;
    uint32_t afrh;
} volatile* gpio_dev_t;

#define _GPIOA  ((gpio_dev_t) (0x40020000))
#define _GPIOB  ((gpio_dev_t) (0x40020400))
#define _GPIOC  ((gpio_dev_t) (0x40020800))
#define _GPIOD  ((gpio_dev_t) (0x40020C00))
#define _GPIOE  ((gpio_dev_t) (0x40021000))
#define _GPIOF  ((gpio_dev_t) (0x40021400))
#define _GPIOG  ((gpio_dev_t) (0x40021800))
#define _GPIOH  ((gpio_dev_t) (0x40021C00))

static const gpio_dev_t GPIO_LL_GPIOx[] = {
    _GPIOA,
    _GPIOB,
    _GPIOC,
    _GPIOD,
    _GPIOE,
    _GPIOF,
    _GPIOG,
    _GPIOH
};

#define GPIO_LL_PORTS ((sizeof(GPIO_LL_GPIOx)) / sizeof(gpio_dev_t))

#define GPIO_LL_GET_HW(num) (((num) < GPIO_LL_PORTS) ? GPIO_LL_GPIOx[(num)] : NULL)

#endif