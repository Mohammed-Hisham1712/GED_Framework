#ifndef __EXTI_HAL_H__

#define __EXTI_HAL_H__

#include "exti_types.h"

#include <stdint.h>



#define EXTI_HAL_GET_HW(context, num)    ((context)->dev = EXTI_LL_GET_HW(num))

typedef void (*exti_hal_irq_callback_t) (void);

typedef struct 
{
    exti_dev_t dev;
}exti_hal_context_t;

/**
 * @brief Init EXTI peripheral
 * 
 * @param hal
 */
void exti_hal_init(exti_hal_context_t* hal);

/**
 * @brief Enable external intterupt for the give line.
 * @note  The function also enables the corresponding NVIC interrupt
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_enable_interrupt(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Disable external intterupt for the give line.
 * @note  The function also disables the corresponding NVIC interrupt line
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_disable_interrupt(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Set triggering edge to be the rising edge
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_enable_rising_edge(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Set triggering edge to be the falling edge
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_enable_falling_edge(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Disable rising edge trigger
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_disable_rising_edge(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Disable falling edge trigger
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_disable_falling_edge(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief Select GPIO port connected to the interrupt line
 * 
 * @param hal 
 * @param line 
 * @param port 
 */
void exti_hal_set_port(exti_hal_context_t* hal, exti_line_t line, exti_port_t port);

/**
 * @brief Clear interrupt pending bit
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_clear_pending(exti_hal_context_t* hal, exti_line_t line);

/**
 * @brief 
 * 
 * @param hal 
 * @param line 
 * @param callback 
 */
void exti_hal_register_callback
            (exti_hal_context_t* hal, exti_line_t line, exti_hal_irq_callback_t callback);

/**
 * @brief 
 * 
 * @param hal 
 * @param line 
 */
void exti_hal_unregister_callback(exti_hal_context_t* hal, exti_line_t line);

#endif