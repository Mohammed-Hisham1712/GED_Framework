#ifndef _GPIO_TYPES_H_
#define _GPIO_TYPES_H_

#include "gpio_periph.h"

/**
 * @brief This file contains all the types that are used by the HAL layer
 *          and also exposed to the Application layer
 */

/**
 * @brief GPIO pin numbers
 * 
 */
typedef enum
{
    GPIO_PIN_0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_8,
    GPIO_PIN_9,
    GPIO_PIN_10,
    GPIO_PIN_11,
    GPIO_PIN_12,
    GPIO_PIN_13,
    GPIO_PIN_14,
    GPIO_PIN_15,
    GPIO_PIN_ALL
} gpio_pin_t;

/**
 * @brief GPIO ports available in this MCU
 * 
 */
typedef enum
{
    GPIOA = 0,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
    GPIOF,
    GPIOG,
    GPIOH,
    GPIO_PORT_ALL
} gpio_port_t;

#endif
