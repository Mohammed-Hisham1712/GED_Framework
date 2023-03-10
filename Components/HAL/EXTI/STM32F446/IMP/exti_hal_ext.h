#ifndef __EXTI_HAL_EXTI_H__

#define __EXTI_HAL_EXTI_H__

#include "exti_hal.h"

#define EXTI_IRQ_LINES      16

uint32_t exti_hal_is_interrupt_enabled(exti_hal_context_t* hal, exti_line_t line);
uint32_t exti_hal_get_interrupt_mask(exti_hal_context_t* hal);
void exti_hal_enable_event(exti_hal_context_t* hal, exti_line_t line);
void exti_hal_disable_event(exti_hal_context_t* hal, exti_line_t line);
uint32_t exti_hal_is_event_enabled(exti_hal_context_t* hal, exti_line_t line);
uint32_t exti_hal_get_event_mask(exti_hal_context_t* hal);
uint32_t exti_hal_is_rising_enabled(exti_hal_context_t* hal, exti_line_t line);
uint32_t exti_hal_is_falling_enabled(exti_hal_context_t* hal, exti_line_t line);
void exti_hal_generate_sw_interrupt(exti_hal_context_t* hal, exti_line_t line);

void exti0_irq_callback(void);
void exti1_irq_callback(void);
void exti2_irq_callback(void);
void exti3_irq_callback(void);
void exti4_irq_callback(void);
void exti9_5_irq_callback(void);
void exti15_10_irq_callback(void);

#endif