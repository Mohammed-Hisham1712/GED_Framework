#include "gpio.h"

#include "gpio_hal.h"
#include "exti_hal.h"
#include "assert.h"

#include <stdint.h>
#include <stddef.h>


error_t gpio_init(gpio_port_t port, const gpio_config_t* config)
{
    gpio_hal_context_t gpio_hal;
    exti_hal_context_t exti_hal;
    uint32_t pin_indx;

    ASSERT(port < GPIO_PORT_ALL);
    ASSERT(config && config->pin_mask);
    
    GPIO_HAL_GET_HW(&gpio_hal, port);

    gpio_hal_init(port);

    pin_indx = 0;
    do
    {
        if((config->pin_mask >> pin_indx) & 0x01)
        {
            if((config->mode & MODE_INPUT) != 0x00)
            {
                gpio_hal_set_mode_input(&gpio_hal, pin_indx);
            }

            else if((config->mode & MODE_OUTPUT) != 0x00)
            {
                if((config->mode & OUTPUT_PP) != 0x00)
                {
                    gpio_hal_set_mode_output_pp(&gpio_hal, pin_indx);
                }
                else
                {
                    gpio_hal_set_mode_output_od(&gpio_hal, pin_indx);
                }
            }

            if((config->mode & MODE_MASK) != MODE_ANALOG)
            {
                
                if(config->pull == GPIO_PULLUP)
                {
                    gpio_hal_set_pull_up(&gpio_hal, pin_indx);
                }
                else if(config->pull == GPIO_PULLDOWN)
                {
                    gpio_hal_set_pull_down(&gpio_hal, pin_indx);
                }
                else
                {
                    gpio_hal_set_pull_disable(&gpio_hal, pin_indx);
                }
            }
            else
            {
                gpio_hal_set_mode_analog(&gpio_hal, pin_indx);
            }

            /* EXTI mode */
            if((config->mode & MODE_INTERRUPT) != 0x00)
            {
                EXTI_HAL_GET_HW(&exti_hal, EXTI);
                
                exti_hal_init(&exti_hal);
                exti_hal_set_port(&exti_hal, EXTI_GET_LINE(pin_indx), EXTI_GET_PORT(port));
                
                if((config->mode & TRIGGER_RISING) != 0x00)
                {
                    exti_hal_enable_rising_edge(&exti_hal, EXTI_GET_LINE(pin_indx));
                }
                else
                {
                    exti_hal_disable_rising_edge(&exti_hal, EXTI_GET_LINE(pin_indx));
                }

                if((config->mode & TRIGGER_FALLING) != 0x00)
                {
                    exti_hal_enable_falling_edge(&exti_hal, EXTI_GET_LINE(pin_indx));
                }
                else
                {
                    exti_hal_disable_falling_edge(&exti_hal, EXTI_GET_LINE(pin_indx));
                }

                if(config->irq_callback)
                {
                    exti_hal_register_callback(&exti_hal, pin_indx, config->irq_callback);
                }

                exti_hal_enable_interrupt(&exti_hal, EXTI_GET_LINE(pin_indx));
            }
        }

        pin_indx++;
    } while (pin_indx < GPIO_PIN_ALL);
    
    return OK;
}

uint32_t gpio_read_pin(gpio_port_t port, gpio_pin_t pin)
{
    gpio_hal_context_t gpio_hal;
    
    GPIO_HAL_GET_HW(&gpio_hal, port);

    return gpio_hal_in_read_pin(&gpio_hal, pin);
}

void gpio_write_pin(gpio_port_t port, gpio_pin_t pin, gpio_pin_state_t pin_state)
{
    gpio_hal_context_t gpio_hal;

    GPIO_HAL_GET_HW(&gpio_hal, port);

    if(pin_state == GPIO_PIN_SET)
    {
        gpio_hal_out_set_pin(&gpio_hal, pin);
    }
    else
    {
        gpio_hal_out_reset_pin(&gpio_hal, pin);
    }
}

void gpio_toggle_pin(gpio_port_t port, gpio_pin_t pin)
{
    gpio_hal_context_t gpio_hal;

    GPIO_HAL_GET_HW(&gpio_hal, port);

    gpio_hal_out_toggle_pin(&gpio_hal, pin);
}