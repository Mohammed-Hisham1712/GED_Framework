#ifndef _GPIO_H_

#define _GPIO_H_

#include "gpio_hal.h"
#include "types.h"

#include "stdint.h"
#include "stddef.h"

#define GPIO_PIN_MASK(pin)  BIT((pin))

#define MODE_INPUT          0x00000001U
#define MODE_OUTPUT         0x00000002U
#define MODE_ANALOG         0x00000004U
#define MODE_INTERRUPT      0x00000008U
#define MODE_MASK           0x0000000FU

#define OUTPUT_PP           0x00000010U
#define OUTPUT_OD           0x00000020U
#define OUTPUT_TYPE_MASK    0x00000030U

#define TRIGGER_RISING      0x00000100U
#define TRIGGER_FALLING     0x00000200U
#define TRIGGER_EDGE_MASK   0x00000300U

#define GPIO_MODE_INPUT             (MODE_INPUT)
#define GPIO_MODE_OUTPUT_PP         (MODE_OUTPUT | OUTPUT_PP)
#define GPIO_MODE_OUTPUT_OD         (MODE_OUTPUT | OUTPUT_OD)
#define GPIO_MODE_ANALOG            (MODE_ANALOG)
#define GPIO_MODE_IT_RISING         (MODE_INPUT | MODE_INTERRUPT | TRIGGER_RISING)
#define GPIO_MODE_IT_FALLING        (MODE_INPUT | MODE_INTERRUPT | TRIGGER_FALLING)
#define GPIO_MODE_IT_RISING_FALLING (MODE_INPUT | MODE_INTERRUPT | TRIGGER_RISING | \
                                                                    TRIGGER_FALLING)

#define GPIO_NOPULL         0x00000000U
#define GPIO_PULLUP         0x00000001U
#define GPIO_PULLDOWN       0x00000002U

typedef void (*gpio_callback_t) (void);

typedef enum
{
    GPIO_PIN_RESET = 0,
    GPIO_PIN_SET = 1
} gpio_pin_state_t;

typedef struct 
{
    uint32_t pin_mask;
    uint32_t mode;
    uint32_t pull;
    gpio_callback_t irq_callback;
} gpio_config_t;

/**
 * @brief Set GPIO pin configuration as specified in config argument and
 *        enable GPIO clock if required. 
 * 
 * @param port port where the pins are to be configured. Should be a value from gpio_port_t
 * @param config pointer to gpio_config_t
 * @return error_t OK/FAILED
 * 
 * @example gpio_config_t gpio_config = {
 *              .pin_mask = GPIO_PIN_MASK(GPIO_PIN_5) | GPIO_PIN_MASK(GPIO_PIN_10),
 *              .mode = GPIO_MODE_INPUT,
 *              .pull = GPIO_PULLUP};
 *          gpio_init(GPIOA, &gpio_config);
 */
error_t gpio_init(gpio_port_t port, const gpio_config_t* config); 

/**
 * @brief read GPIO pin state
 * 
 * @param port port that the pin belongs to i.e GPIOA
 * @param pin pin number as in gpio_pin_t
 * @return uint32_t pin state 1: High, 0:Low
 * 
 * @example uint32_t pin_state = gpio_read_pin(GPIOA, GPIO_PIN_15);
 */
uint32_t gpio_read_pin(gpio_port_t port, gpio_pin_t pin);

/**
 * @brief Set/Reset GPIO pin
 * 
 * @param port port that the pin belongs to i.e GPIOA.
 * @param pin pin number as in gpio_pin_t
 * @param state GPIO_PIN_RESET/GPIO_PIN_SET
 * 
 * @example gpio_write_pin(GPIOA, GPIO_PIN_10, GPIO_PIN_RESET); //Clear GPIO pin
 */
void gpio_write_pin(gpio_port_t port, gpio_pin_t pin, gpio_pin_state_t state);
/**
 * @brief toggle GPIO pin. If high make it low, if low make it high
 * 
 * @param port port that the pin belongs to i.e GPIOA
 * @param pin pin number as in gpio_pin_t
 * @return void
 * 
 * @example gpio_read_pin(GPIOA, GPIO_PIN_5);
 */
void gpio_toggle_pin(gpio_port_t port, gpio_pin_t pin);

#endif