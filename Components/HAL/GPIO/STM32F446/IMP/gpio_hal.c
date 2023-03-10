#include "gpio_hal_ext.h"
#include "gpio_ll.h"

#include "rcc_hal_ext.h"
#include "assert.h"

#include "types.h"

#define GPIO_IS_PIN(pin)        ((pin) < GPIO_PIN_ALL)

error_t gpio_hal_init(gpio_port_t port)
{
    rcc_hal_context_t rcc_context;

    ASSERT(port < GPIO_PORT_ALL);

    RCC_HAL_GET_HW(&rcc_context, RCC);
    ASSERT(rcc_context.dev);

    rcc_hal_ahb1_en_clk(&rcc_context, RCC_HAL_AHB1_GPIO(port));
    
    return OK;
}

error_t gpio_hal_lock(gpio_hal_context_t* hal, uint32_t pin_mask)
{
    ASSERT(hal);

    gpio_ll_lock_seq_write(hal->dev, pin_mask);

    if(!gpio_ll_lock_read(hal->dev))
    {
        return FAILED;
    }
    
    return OK;
}

void gpio_hal_set_mode_input(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_input(hal->dev, pin);
}

void gpio_hal_set_mode_output_pp(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_output(hal->dev, pin); 
    gpio_ll_set_output_type_pp(hal->dev, pin);
}

void gpio_hal_set_mode_output_od(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_output(hal->dev, pin); 
    gpio_ll_set_output_type_od(hal->dev, pin);
}

void gpio_hal_set_mode_analog(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_analog(hal->dev, pin);
}

void gpio_hal_set_mode_alternate_pp(gpio_hal_context_t* hal, gpio_pin_t pin, uint8_t func)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_af(hal->dev, pin);
    gpio_ll_set_output_type_pp(hal->dev, pin);
    gpio_ll_set_alternate_func(hal->dev, pin, func);
}

void gpio_hal_set_mode_alternate_od(gpio_hal_context_t* hal, gpio_pin_t pin, uint8_t func)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_mode_af(hal->dev, pin);
    gpio_ll_set_output_type_od(hal->dev, pin);
    gpio_ll_set_alternate_func(hal->dev, pin, func);
}

void gpio_hal_set_pull_disable(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_pull_disable(hal->dev, pin);
}

void gpio_hal_set_pull_up(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_pull_up(hal->dev, pin);
}

void gpio_hal_set_pull_down(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);
    ASSERT(GPIO_IS_PIN(pin));

    gpio_ll_set_pull_down(hal->dev, pin);
}

uint32_t gpio_hal_in_read_pin(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);

    return gpio_ll_in_read_pin(hal->dev, pin);
}

uint32_t gpio_hal_in_read_port(gpio_hal_context_t* hal)
{
    ASSERT(hal);

    return gpio_ll_in_read_port(hal->dev);
}

void gpio_hal_out_reset_pin(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);

    gpio_ll_out_reset_pin(hal->dev, pin);
}

void gpio_hal_out_set_pin(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);

    gpio_ll_out_set_pin(hal->dev, pin);
}

void gpio_hal_out_toggle_pin(gpio_hal_context_t* hal, gpio_pin_t pin)
{
    ASSERT(hal);

    gpio_ll_out_toggle_pin(hal->dev, pin);
}

void gpio_hal_out_reset_mask(gpio_hal_context_t* hal, uint32_t mask)
{
    ASSERT(hal);

    gpio_ll_out_reset_mask(hal->dev, mask);
}

void gpio_hal_out_set_mask(gpio_hal_context_t* hal, uint32_t mask)
{
    ASSERT(hal);

    gpio_ll_out_set_mask(hal->dev, mask);
}

void gpio_hal_out_toggle_mask(gpio_hal_context_t* hal, uint32_t mask)
{
    ASSERT(hal);

    gpio_ll_out_toggle_mask(hal->dev, mask);
}
