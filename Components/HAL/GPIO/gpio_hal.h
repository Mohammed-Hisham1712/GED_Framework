#ifndef _GPIO_HAL_H

#define _GPIO_HAL_H

#include "gpio_types.h"


#include "types.h"

/**
 * @brief Stucture data type containing one element, dev that points to the
 *          start address of the peripheral in memory map of the device
 */
typedef struct
{
    gpio_dev_t dev;
} gpio_hal_context_t;

/**
 * @brief Macro to get a pointer to GPIO registers
 * @param context a pointer gpio_hal_context_t type
 * @param num gpio port in gpio_port_t enum
 */
#define GPIO_HAL_GET_HW(context, num)     ((context)->dev = GPIO_LL_GET_HW(num))


/**
 * @brief Init GPIO port
 * @param hal pointer to gpio_hal_context_t
 * @return 0:ok, -1: fail
 */
error_t gpio_hal_init(gpio_port_t port);

/**
 * @brief Lock pins specified in pin_mask.
 * 
 * @param hal pointer to gpio_hal_context_t
 * @param pin_mask
 * @return error_t OK/FAILED 
 */
error_t gpio_hal_lock(gpio_hal_context_t* hal, uint32_t pin_mask);

/**
 * @brief set pin mode to input
 * @param hal pointer to gpio_hal_context_t
 * @param pin pin to set as input. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_mode_input(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief set pin mode to output push-pull
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_mode_output_pp(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief set pin mode to output open-drain
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_mode_output_od(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief set pin mode to analog
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_mode_analog(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief disable pull up/down
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_pull_disable(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief enable pull-up
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_pull_up(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief enable pull-down
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_set_pull_down(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief get pin input value
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 * @return 1: high, 0: low
 */
uint32_t gpio_hal_in_read_pin(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief get port input value
 * @param hal pointer to gpio_hal_context_t
 * @return port input value
 */
uint32_t gpio_hal_in_read_port(gpio_hal_context_t* hal);

/**
 * @brief set pin to low
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_out_reset_pin(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief set pin to high
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_out_set_pin(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief toggle output pin
 * @param hal pointer to gpio_hal_context_t
 * @param pin GPIO pin. From GPIO_PIN_0 to GPIO_PIN_15
 */
void gpio_hal_out_toggle_pin(gpio_hal_context_t* hal, gpio_pin_t pin);

/**
 * @brief set all pins in mask to low
 * @param hal pointer to gpio_hal_context_t
 * @param mask GPIO pin mask
 */
void gpio_hal_out_reset_mask(gpio_hal_context_t* hal, uint32_t mask);

/**
 * @brief set all pins in mask to high
 * @param hal pointer to gpio_hal_context_t
 * @param mask GPIO pin mask
 */
void gpio_hal_out_set_mask(gpio_hal_context_t* hal, uint32_t mask);

/**
 * @brief toggle all pins in mask
 * @param hal pointer to gpio_hal_context_t
 * @param mask GPIO pin mask
 */
void gpio_hal_out_toggle_mask(gpio_hal_context_t* hal, uint32_t mask);


#endif