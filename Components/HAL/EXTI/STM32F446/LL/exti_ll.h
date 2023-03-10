#ifndef __EXTI_LL_H__

#define __EXTI_LL_H__

/**
 * @brief External interrupt low level functions
 * 
 * @note This file shouldn't be included in Application code
 * 
 * @author Mohammed Hisham
 * @date Dec 27, 2022
 */

#include "exti_periph.h"

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

/**
 * @brief Enable interrupt in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * 
 */
static inline void exti_ll_enable_interrupt(exti_dev_t hw, uint32_t exti_num)
{
    REG_SET_BIT(hw->imr, EXTI_BIT(exti_num));
}

/**
 * @brief Disable interrupt in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 */
static inline void exti_ll_disable_interrupt(exti_dev_t hw, uint32_t exti_num)
{
    REG_CLR_BIT(hw->imr, EXTI_BIT(exti_num));
}

/**
 * @brief Get interrupt line enabled state
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * @return uint32_t 1: enabled, 0: disabled
 */
static inline uint32_t exti_ll_is_interrupt_enabled(exti_dev_t hw, uint32_t exti_num)
{
    return REG_GET_BIT(hw->imr, EXTI_BIT(exti_num)) ? 1 : 0;
}

/**
 * @brief Get interrupt mask register
 * 
 * @param hw pointer to EXTI start address
 * @return uint32_t EXTI_IMR value
 */
static inline uint32_t exti_ll_get_interrupt_mask(exti_dev_t hw)
{
    return hw->imr;
}

/**
 * @brief Enable event in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * 
 */
static inline void exti_ll_enable_event(exti_dev_t hw, uint32_t exti_num)
{
    REG_SET_BIT(hw->emr, EXTI_BIT(exti_num));
}

/**
 * @brief Disable event in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 */
static inline void exti_ll_disable_event(exti_dev_t hw, uint32_t exti_num)
{
    REG_CLR_BIT(hw->emr, EXTI_BIT(exti_num));
}

/**
 * @brief Get event enabled state
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * @return uint32_t 1: enabled, 0: disabled
 */
static inline uint32_t exti_ll_is_event_enabled(exti_dev_t hw, uint32_t exti_num)
{
    return REG_GET_BIT(hw->emr, EXTI_BIT(exti_num)) ? 1 : 0;
}

/**
 * @brief Get event mask register
 * 
 * @param hw pointer to EXTI start address
 * @return uint32_t EXTI_EMR value
 */
static inline uint32_t exti_ll_get_event_mask(exti_dev_t hw)
{
    return hw->emr;
}
/**
 * @brief Enable rising edge in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22 execluding EXTI19
 * 
 */
static inline void exti_ll_enable_rising(exti_dev_t hw, uint32_t exti_num)
{
    REG_SET_BIT(hw->rtsr, EXTI_BIT(exti_num));
}

/**
 * @brief Disable rising edge in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22 execluding EXTI19
 */
static inline void exti_ll_disable_rising(exti_dev_t hw, uint32_t exti_num)
{
    REG_CLR_BIT(hw->rtsr, EXTI_BIT(exti_num));
}

/**
 * @brief Get rising edge enable state
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * @return uint32_t 1: enabled, 0: disabled
 */
static inline uint32_t exti_ll_is_rising_enabled(exti_dev_t hw, uint32_t exti_num)
{
    return REG_GET_BIT(hw->rtsr, EXTI_BIT(exti_num)) ? 1 : 0;
}

/**
 * @brief Enable falling edge in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22 execluding EXTI19
 * 
 */
static inline void exti_ll_enable_falling(exti_dev_t hw, uint32_t exti_num)
{
    REG_SET_BIT(hw->ftsr, EXTI_BIT(exti_num));
}

/**
 * @brief Disable falling edge in EXTIx
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22 execluding EXTI19
 */
static inline void exti_ll_disable_falling(exti_dev_t hw, uint32_t exti_num)
{
    REG_CLR_BIT(hw->ftsr, EXTI_BIT(exti_num));
}

/**
 * @brief Get falling edge enable state
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * @return uint32_t 1: enabled, 0: disabled
 */
static inline uint32_t exti_ll_is_falling_enabled(exti_dev_t hw, uint32_t exti_num)
{
    return REG_GET_BIT(hw->ftsr, EXTI_BIT(exti_num)) ? 1 : 0;
}

/**
 * @brief Generate a S.W interrupt on EXTI
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 */
static inline void exti_ll_generate_sw_interrupt(exti_dev_t hw, uint32_t exti_num)
{
    hw->swier = EXTI_BIT(exti_num);
}

/**
 * @brief Clear pending bit for EXTIn
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 */
static inline void exti_ll_clear_pending(exti_dev_t hw, uint32_t exti_num)
{
    hw->pr = EXTI_BIT(exti_num);
}

/**
 * @brief Get pending status for EXTIn
 * 
 * @param hw pointer to EXTI start address
 * @param exti_num EXTI number from EXTI0-EXTI22
 * 
 * @return uint32_t 1:Interrupt is pending 0:otherwise
 */
static inline uint32_t exti_ll_is_pending(exti_dev_t hw, uint32_t exti_num)
{
    return REG_GET_BIT(hw->pr, EXTI_BIT(exti_num)) ? 1 : 0;
}

static inline uint32_t exti_ll_get_pending(exti_dev_t hw)
{
    return hw->pr;
}

#endif
