#ifndef __GPIO_HAL_EXT_H__

#define __GPIO_HAL_EXT_H__

#include "gpio_hal.h"

/**
 * @brief 
 * 
 * @param hal 
 * @param pin 
 */
void gpio_hal_set_mode_alternate_pp(gpio_hal_context_t* hal, gpio_pin_t pin, uint8_t func);

/**
 * @brief 
 * 
 * @param hal 
 * @param pin 
 */
void gpio_hal_set_mode_alternate_od(gpio_hal_context_t* hal, gpio_pin_t pin, uint8_t func);

#endif