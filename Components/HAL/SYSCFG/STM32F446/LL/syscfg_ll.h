#ifndef __SYSCFG_LL_H__

#define __SYSCFG_LL_H__

/**
 * @brief SYSCFG peripheral low level functions
 * 
 * @note Don't include in your Application
 * 
 * @author Mohammed Hisham
 * @date Dec. 27, 2022
 */

#include "syscfg_periph.h"

#include "bit_math.h"

#include <stdint.h>

/**
 * @brief Set GPIO port for EXTI interrupt
 * 
 * @param hw Pointer to start address of SYSCFG
 * @param exti_num  EXTI number from 0 to 15
 * @param gpio_port GPIO port to select as source of interrupt
 *                  From EXTI_GPIO_A to EXTI_GPIO_H
 * @note            EXTI_GPIO_H is available ONLY for exti0-exti3
 */
static inline void syscfg_ll_set_exti_port
                    (syscfg_dev_t hw, uint32_t exti_num, uint32_t gpio_port)
{
    volatile uint32_t* exticr;

    exticr = SYSCFG_EXTICR_REG(hw, exti_num);
    REG_WRITE_BITS(*exticr, SYSCFG_EXTICR_S(exti_num), SYSCFG_EXTICR_M, gpio_port);
}

/**
 * @brief Get GPIO port configured for EXTI interrupt
 * 
 * @param hw Pointer to start address of SYSCFG
 * @param exti_num EXTI number from 0 to 15
 * @return syscfg_ll_exti_gpio_t GPIO port from EXTI_GPIO_A to EXTI_GPIO_H
 */
static inline uint32_t syscfg_ll_get_exti_port(syscfg_dev_t hw, uint32_t exti_num)
{
    volatile uint32_t* exticr;

    exticr = SYSCFG_EXTICR_REG(hw, exti_num);

    return REG_GET_BITS(*exticr, SYSCFG_EXTICR_S(exti_num), SYSCFG_EXTICR_M);
}

#endif