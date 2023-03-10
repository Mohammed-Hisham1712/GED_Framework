#ifndef _GPIO_LL_H_
#define _GPIO_LL_H_

#include "gpio_periph.h"

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    LL_GPIO_MODE_INPUT = 0,
    LL_GPIO_MODE_OUTPUT,
    LL_GPIO_MODE_ALTERNATE,
    LL_GPIO_MODE_ANALOG
} gpio_ll_mode_t;

typedef enum
{
    LL_GPIO_OUTPUT_PUSHPULL,
    LL_GPIO_OUTPUT_OPENDRAIN
} gpio_ll_output_t;

typedef enum
{
    LL_GPIO_SPEED_FREQ_LOW = 0,
    LL_GPIO_SPEED_FREQ_MEDIUM,
    LL_GPIO_SPEED_FREQ_HIGH,
    LL_GPIO_SPEED_FREQ_VERY_HIGH
} gpio_ll_speed_t;

typedef enum
{
    LL_GPIO_PULL_NO = 0,
    LL_GPIO_PULL_UP,
    LL_GPIO_PULL_DOWN
} gpio_ll_pull_t;

/**
* @brief Set GPIO pin to input mode
* @param gpio_dev_t pointer to start address of GPIO
* @param pin GPIO pin from GPIO0-GPIO15
*/
static inline void gpio_ll_set_mode_input(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->moder, GPIO_MODE_S(pin), GPIO_MODE_M, LL_GPIO_MODE_INPUT);
}

/**
 * @brief Set GPIO pin to output mode
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_mode_output(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->moder, GPIO_MODE_S(pin), GPIO_MODE_M, LL_GPIO_MODE_OUTPUT);
}

/**
 * @brief Set GPIO pin to AF mode
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_mode_af(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->moder, GPIO_MODE_S(pin), GPIO_MODE_M, LL_GPIO_MODE_ALTERNATE);
}

/**
 * @brief Set GPIO pin to analog mode
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_mode_analog(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->moder, GPIO_MODE_S(pin), GPIO_MODE_M, LL_GPIO_MODE_ANALOG);
}

/**
 * @brief Get GPIO pin mode
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 * 
 * @return GPIO mode. 0x00 = input, 0x01 = output, 0x10 = AF, 0x11 = Analog
 */
static inline uint32_t gpio_ll_get_mode(gpio_dev_t hw, uint8_t pin)
{
    return REG_GET_BITS(hw->moder, GPIO_MODE_S(pin), GPIO_MODE_M);
}

/**
 * @brief Set GPIO pin output to Push Pull
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_output_type_pp(gpio_dev_t hw, uint8_t pin)
{
    REG_CLR_BIT(hw->otyper, BIT(pin));
}

/**
 * @brief Set GPIO pin output to Open Drain
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_output_type_od(gpio_dev_t hw, uint8_t pin)
{
    REG_SET_BIT(hw->otyper, BIT(pin));
}

/**
 * @brief Get GPIO pin output to Open Drain
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 * 
 * @return GPIO output mode. 0 = push pull, 1 = open drain
 */
static inline uint32_t gpio_ll_get_output_type(gpio_dev_t hw, uint8_t pin)
{
    return REG_GET_BIT(hw->otyper, BIT(pin)) ? LL_GPIO_OUTPUT_OPENDRAIN : LL_GPIO_OUTPUT_PUSHPULL;
}

/**
 * @brief Set GPIO pin output speed to low
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_speed_low(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_SPEED_S(pin), GPIO_SPEED_M, LL_GPIO_SPEED_FREQ_LOW);
}

/**
 * @brief Set GPIO pin output speed to Medium
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_speed_medium(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_SPEED_S(pin), GPIO_SPEED_M, LL_GPIO_SPEED_FREQ_MEDIUM);
}

/**
 * @brief Set GPIO pin output speed to High
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_speed_high(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_SPEED_S(pin), GPIO_SPEED_M, LL_GPIO_SPEED_FREQ_HIGH);
}

/**
 * @brief Set GPIO pin output speed to very high
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_speed_very_high(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_SPEED_S(pin), GPIO_SPEED_M, LL_GPIO_SPEED_FREQ_VERY_HIGH);
}

/**
 * @brief Get GPIO pin output speed
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 * 
 * @return GPIO output speed. 0x00 = low, 0x01 = medium, 0x10 = high, 0x11 = very high
 */
static inline uint32_t gpio_ll_get_output_speed(gpio_dev_t hw, uint8_t pin)
{
    return REG_GET_BITS(hw->ospeedr, GPIO_SPEED_S(pin), GPIO_SPEED_M);
}

/**
 * @brief Disable GPIO pin pull functionality
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_pull_disable(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->pupdr, GPIO_PUPD_S(pin), GPIO_PUPD_M, LL_GPIO_PULL_NO);
}

/**
 * @brief Enable GPIO pin pull up
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_pull_up(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_PUPD_S(pin), GPIO_PUPD_M, LL_GPIO_PULL_UP);
}

/**
 * @brief Enable GPIO pin pull down
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */
static inline void gpio_ll_set_pull_down(gpio_dev_t hw, uint8_t pin)
{
    REG_WRITE_BITS(hw->ospeedr, GPIO_PUPD_S(pin), GPIO_PUPD_M, LL_GPIO_PULL_DOWN);
}

/**
 * @brief Get GPIO pin pull state
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 * 
 * @return GPIO pull. 0x00 = no, 0x01 = up, 0x10 = down
 */
static inline uint32_t gpio_ll_get_pull(gpio_dev_t hw, uint8_t pin)
{
    return REG_GET_BITS(hw->pupdr, GPIO_PUPD_S(pin), GPIO_PUPD_M);
}

/**
 * @brief Get GPIO pin input value
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 * 
 * @return GPIO input level 0 = low, 1 = high
 */

static inline uint32_t gpio_ll_in_read_pin(gpio_dev_t hw, uint8_t pin)
{
    return (REG_GET_BIT(hw->idr, BIT(pin)) ? 1 : 0);
}

/**
 * @brief Get GPIO port input value
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * 
 * @return GPIO input port
 */

static inline uint32_t gpio_ll_in_read_port(gpio_dev_t hw)
{
    return hw->idr;
}

/**
 * @brief Set GPIO pin output low. Atomic operation
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */

static inline void gpio_ll_out_reset_pin(gpio_dev_t hw, uint8_t pin)
{
    hw->bsrr = GPIO_BSR_RESET_S(pin);
}

/**
 * @brief Set GPIO pin output high. Atomic operation
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */

static inline void gpio_ll_out_set_pin(gpio_dev_t hw, uint8_t pin)
{
    hw->bsrr = BIT(pin);
}

/**
 * @brief Toggle GPIO pin output high
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin GPIO pin from GPIO0-GPIO15
 */

static inline void gpio_ll_out_toggle_pin(gpio_dev_t hw, uint8_t pin)
{
    /* Uses BSRR to allow atomic read-modify-access. 
    ** There's no risk if an interrupt occured during
    ** the function call
    */

    uint32_t odr = hw->odr;

    hw->bsrr = ((odr & BIT(pin)) << GPIO_LL_PINS) | (~odr & BIT(pin));
}

/**
 * @brief Reset GPIO pins in mask
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin_mask GPIO pins to reset. From GPIO0-GPIO15
 */

static inline void gpio_ll_out_reset_mask(gpio_dev_t hw, uint32_t pin_mask)
{
    hw->bsrr = GPIO_BSR_RESET_M(pin_mask);
}

/**
 * @brief Set GPIO pins in mask
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin_mask GPIO pins to set. From GPIO0-GPIO15
 */

static inline void gpio_ll_out_set_mask(gpio_dev_t hw, uint32_t pin_mask)
{
    hw->bsrr = pin_mask;
}

/**
 * @brief Toggle GPIO pins in mask
 * 
 * @param gpio_dev_t pointer to start address of GPIO
 * @param pin_mask GPIO pins to toggle. From GPIO0-GPIO15
 */

static inline void gpio_ll_out_toggle_mask(gpio_dev_t hw, uint32_t pin_mask)
{
    /* Uses BSRR to allow atomic read-modify-access. 
    ** There's no risk if an interrupt occured during
    ** the function call
    */

    uint32_t odr = hw->odr;
    
    hw->bsrr = ((odr & pin_mask) << GPIO_LL_PINS) | (~odr & pin_mask);
}

/**
 * @brief Apply pin lock sequence.
 * 
 * @param hw pointer to start address of GPIO
 * @param pin_mask GPIO pins to toggle. From GPIO0-GPIO15
 */
static inline void gpio_ll_lock_seq_write(gpio_dev_t hw, uint32_t pin_mask)
{
    hw->lckr = BIT(16) | pin_mask;
    hw->lckr = pin_mask;
    hw->lckr = BIT(16) | pin_mask;
    (void) hw->lckr;
}

/**
 * @brief Get the value of lckk bit in GPIO_LCKR. 
 *        Can be used to varify successful lock sequence
 * 
 * @param hw pointer to start address of GPIO
 * @return 1: Lock applied, 0: No lock
 */
static inline uint32_t gpio_ll_lock_read(gpio_dev_t hw)
{
    return REG_GET_BIT(hw->lckr, BIT(16)) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param pin 
 * @param func 
 */
static inline void gpio_ll_set_alternate_func(gpio_dev_t hw, gpio_pin_t pin, uint8_t func)
{
    if(pin < GPIO_PIN_8)
    {
        REG_WRITE_BITS(hw->afrl, GPIO_AFR_S(pin), GPIO_AFR_M, func);
    }
    else
    {
        REG_WRITE_BITS(hw->afrh, GPIO_AFR_S(pin - 8), GPIO_AFR_M, func);
    }
}

#endif