#include "exti_hal_ext.h"
#include "exti_ll.h"

#include "rcc_hal_ext.h"
#include "syscfg_hal_ext.h"
#include "stm32f446xx.h"
#include "assert.h"

#include <stdint.h>

#define EXTI_IS_LINE(_line)         ((_line) < EXTI_MAX_LINES)
#define EXTI_IS_PIN(_line)          ((_line) <= EXTI_LINE_15)
#define EXTI_IS_PORT(_port)         ((_port) < EXTI_MAX_PORTS)

#define EXTI_IS_EXTI9_5_CLEAR(_exti_lines)      (((_exti_lines) & (EXTI_BIT(EXTI_LINE_5) \
                                                        | EXTI_BIT(EXTI_LINE_6) \
                                                        | EXTI_BIT(EXTI_LINE_7) \
                                                        | EXTI_BIT(EXTI_LINE_8) \
                                                        | EXTI_BIT(EXTI_LINE_9))) == 0X00)

#define EXTI_IS_EXTI15_10_CLEAR(_exti_lines)    (((_exti_lines) & (EXTI_BIT(EXTI_LINE_10) \
                                                        | EXTI_BIT(EXTI_LINE_11) \
                                                        | EXTI_BIT(EXTI_LINE_12) \
                                                        | EXTI_BIT(EXTI_LINE_13) \
                                                        | EXTI_BIT(EXTI_LINE_14) \
                                                        | EXTI_BIT(EXTI_LINE_15))) == 0X00)

static exti_hal_irq_callback_t callback_ptrs[EXTI_IRQ_LINES];

void exti_hal_init(exti_hal_context_t* hal)
{
    (void) hal;
    
    syscfg_hal_init();
}

void exti_hal_enable_interrupt(exti_hal_context_t* hal, exti_line_t line)
{
    IRQn_Type exti;

    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    if(line == EXTI_LINE_0)
    {
        exti = EXTI0_IRQn;
    }
    else if(line == EXTI_LINE_1)
    {
        exti = EXTI1_IRQn;
    }
    else if(line == EXTI_LINE_2)
    {
        exti = EXTI2_IRQn;
    }
    else if(line == EXTI_LINE_3)
    {
        exti = EXTI3_IRQn;
    }
    else if(line == EXTI_LINE_4)
    {
        exti = EXTI4_IRQn;
    }
    else if(line >= EXTI_LINE_5 && line <= EXTI_LINE_9)
    {
        exti = EXTI9_5_IRQn;
    }
    else if(line >= EXTI_LINE_10 && line <= EXTI_LINE_15)
    {
        exti = EXTI15_10_IRQn;
    }

    if(EXTI_IS_PIN(line))
    {
        NVIC_ClearPendingIRQ(exti);
        NVIC_EnableIRQ(exti);
    }

    exti_ll_enable_interrupt(hal->dev, line);

}

void exti_hal_disable_interrupt(exti_hal_context_t* hal, exti_line_t line)
{
    uint32_t lines_mask;
    IRQn_Type exti_mask;
    uint8_t disable = 0;

    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_disable_interrupt(hal->dev, line);

    lines_mask = exti_ll_get_interrupt_mask(hal->dev);

    if(line == EXTI_LINE_0)
    {
        exti_mask = EXTI0_IRQn;
        disable = 1;
    }
    else if(line == EXTI_LINE_1)
    {
        exti_mask = EXTI1_IRQn;
        disable = 1;
    }
    else if(line == EXTI_LINE_2)
    {
        exti_mask = EXTI2_IRQn;
        disable = 1;
    }
    else if(line == EXTI_LINE_3)
    {
        exti_mask = EXTI3_IRQn;
        disable = 1;
    }
    else if(line == EXTI_LINE_4)
    {
        exti_mask = EXTI4_IRQn;
        disable = 1;
    }
    else if(line >= EXTI_LINE_5 && line <= EXTI_LINE_9)
    {
        if(EXTI_IS_EXTI9_5_CLEAR(lines_mask))
        {
            exti_mask = EXTI9_5_IRQn;
            disable = 1;
        }
    }
    else if(line >= EXTI_LINE_10 && line <= EXTI_LINE_15)
    {
        if(EXTI_IS_EXTI15_10_CLEAR(lines_mask))
        {
            exti_mask = EXTI15_10_IRQn;
            disable = 1;
        }
    }

    if(disable)
    {
        NVIC_DisableIRQ(exti_mask);
    }
}

uint32_t exti_hal_is_interrupt_enabled(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    return exti_ll_is_interrupt_enabled(hal->dev, line);
}

uint32_t exti_hal_get_interrupt_mask(exti_hal_context_t* hal)
{
    ASSERT(hal);

    return exti_ll_get_interrupt_mask(hal->dev);
}

void exti_hal_enable_event(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_enable_event(hal->dev, line);
}

void exti_hal_disable_event(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_disable_event(hal->dev, line);
}

uint32_t exti_hal_is_event_enabled(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    return exti_ll_is_event_enabled(hal->dev, line);
}

uint32_t exti_hal_get_event_mask(exti_hal_context_t* hal)
{
    ASSERT(hal);

    return exti_ll_get_event_mask(hal->dev);
}

void exti_hal_enable_rising_edge(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_enable_rising(hal->dev, line);
}

void exti_hal_enable_falling_edge(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_enable_falling(hal->dev, line);
}

void exti_hal_disable_rising_edge(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_disable_rising(hal->dev, line);
}

void exti_hal_disable_falling_edge(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_disable_falling(hal->dev, line);
}

uint32_t exti_hal_is_rising_enabled(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    return exti_ll_is_rising_enabled(hal->dev, line);
}

uint32_t exti_hal_is_falling_enabled(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    return exti_ll_is_falling_enabled(hal->dev, line);
}

void exti_hal_set_port(exti_hal_context_t* hal, exti_line_t line, exti_port_t port)
{
    syscfg_hal_context_t syscfg;

    ASSERT(hal);
    ASSERT(EXTI_IS_PIN(line));
    ASSERT(EXTI_IS_PORT(port));
    ASSERT(!(port > EXTI_PORT_G && line > EXTI_LINE_3));
    
    SYSCFG_HAL_GET_HW(&syscfg);
    syscfg_hal_set_exti_port(&syscfg, line, port);
}

void exti_hal_clear_pending(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));
    
    exti_ll_clear_pending(hal->dev, line);
}

uint32_t exti_hal_is_pending(exti_hal_context_t* hal, exti_line_t line)
{
    return exti_ll_is_pending(hal->dev, line);
}

uint32_t exti_hal_get_pending(exti_hal_context_t* hal)
{
    return exti_ll_get_pending(hal->dev);
}

void exti_hal_generate_sw_interrupt(exti_hal_context_t* hal, exti_line_t line)
{
    ASSERT(hal);
    ASSERT(EXTI_IS_LINE(line));

    exti_ll_generate_sw_interrupt(hal->dev, line);
}

void exti_hal_register_callback
            (exti_hal_context_t* hal, exti_line_t line, exti_hal_irq_callback_t callback)
{
    (void) hal;

    ASSERT(EXTI_IS_PIN(line));
    
    callback_ptrs[line] = callback;
}

void exti_hal_unregister_callback(exti_hal_context_t* hal, exti_line_t line)
{
    (void) hal;

    ASSERT(EXTI_IS_PIN(line));

    callback_ptrs[line] = NULL;
}

/* IRQ handler functions */

void exti0_irq_handler(void)
{
    exti0_irq_callback();
}

void exti1_irq_handler(void)
{
    exti1_irq_callback();
}

void exti2_irq_handler(void)
{
    exti2_irq_callback();
}

void exti3_irq_handler(void)
{
    exti3_irq_callback();
}

void exti4_irq_handler(void)
{
    exti4_irq_callback();
}

void exti9_5_irq_handler(void)
{
    exti9_5_irq_callback();
}

void exti15_10_irq_handler(void)
{
    exti15_10_irq_callback();
}

__WEAK void exti0_irq_callback(void)
{
    if(callback_ptrs[EXTI_LINE_0])
    {
        callback_ptrs[EXTI_LINE_0]();
    }
}

__WEAK void exti1_irq_callback(void)
{
    if(callback_ptrs[EXTI_LINE_1])
    {
        callback_ptrs[EXTI_LINE_1]();
    }
}

__WEAK void exti2_irq_callback(void)
{
    if(callback_ptrs[EXTI_LINE_2])
    {
        callback_ptrs[EXTI_LINE_2]();
    }
}

__WEAK void exti3_irq_callback(void)
{
    if(callback_ptrs[EXTI_LINE_3])
    {
        callback_ptrs[EXTI_LINE_3]();
    }
}

__WEAK void exti4_irq_callback(void)
{
    if(callback_ptrs[EXTI_LINE_4])
    {
        callback_ptrs[EXTI_LINE_4]();
    }
}

__WEAK void exti9_5_irq_callback(void)
{
    exti_hal_context_t hw;
    uint32_t pending_lines;

    hw.dev = EXTI_LL_GET_HW(EXTI);

    pending_lines = exti_hal_get_pending(&hw);

    if(callback_ptrs[EXTI_LINE_5] && (pending_lines & EXTI_BIT(EXTI_LINE_5)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_5]();
        exti_hal_clear_pending(&hw, EXTI_LINE_5);
    }
    else if(callback_ptrs[EXTI_LINE_6] && (pending_lines & EXTI_BIT(EXTI_LINE_6)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_6]();
        exti_hal_clear_pending(&hw, EXTI_LINE_6);
    }
    else if(callback_ptrs[EXTI_LINE_7] && (pending_lines & EXTI_BIT(EXTI_LINE_7)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_7]();
        exti_hal_clear_pending(&hw, EXTI_LINE_7);
    }
    else if(callback_ptrs[EXTI_LINE_8] && (pending_lines & EXTI_BIT(EXTI_LINE_8)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_8]();
        exti_hal_clear_pending(&hw, EXTI_LINE_8);
    }
    else if(callback_ptrs[EXTI_LINE_9] && (pending_lines & EXTI_BIT(EXTI_LINE_9)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_9]();
        exti_hal_clear_pending(&hw, EXTI_LINE_9);
    }
}

__WEAK void exti15_10_irq_callback(void)
{
    exti_hal_context_t hw;
    uint32_t pending_lines;

    hw.dev = EXTI_LL_GET_HW(EXTI);

    pending_lines = exti_hal_get_pending(&hw);

    if(callback_ptrs[EXTI_LINE_10] && (pending_lines & EXTI_BIT(EXTI_LINE_10)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_10]();
        exti_hal_clear_pending(&hw, EXTI_LINE_10);
    }
    else if(callback_ptrs[EXTI_LINE_11] && (pending_lines & EXTI_BIT(EXTI_LINE_11)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_11]();
        exti_hal_clear_pending(&hw, EXTI_LINE_11);
    }
    else if(callback_ptrs[EXTI_LINE_12] && (pending_lines & EXTI_BIT(EXTI_LINE_12)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_12]();
        exti_hal_clear_pending(&hw, EXTI_LINE_12);
    }
    else if(callback_ptrs[EXTI_LINE_13] && (pending_lines & EXTI_BIT(EXTI_LINE_13)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_13]();
        exti_hal_clear_pending(&hw, EXTI_LINE_13);
    }
    else if(callback_ptrs[EXTI_LINE_14] && (pending_lines & EXTI_BIT(EXTI_LINE_14)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_14]();
        exti_hal_clear_pending(&hw, EXTI_LINE_14);
    }
    else if(callback_ptrs[EXTI_LINE_15] && (pending_lines & EXTI_BIT(EXTI_LINE_15)) != 0x00)
    {
        callback_ptrs[EXTI_LINE_15]();
        exti_hal_clear_pending(&hw, EXTI_LINE_15);
    }
}