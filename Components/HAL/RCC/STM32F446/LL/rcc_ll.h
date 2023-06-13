#ifndef _RCC_LL_H_

#define _RCC_LL_H_

#include "rcc_periph.h"

#include "bit_math.h"

#include <stdint.h>

/**
 * @brief Enable periphal clock on AHB1 bus
 * 
 * @param hw pointer to RCC address
 * @param periph peripheral number in RCC_AHB1ENR register
 */
static inline void rcc_ll_ahb1_en_clk(rcc_dev_t hw, uint32_t periph)
{
    REG_SET_BIT(hw->ahb1_enr, BIT(periph));
}

/**
 * @brief Enable periphal clock on APB1 bus
 * 
 * @param hw pointer to RCC address
 * @param periph peripheral number in RCC_APB1ENR register
 */
static inline void rcc_ll_apb1_en_clk(rcc_dev_t hw, uint32_t periph)
{
    REG_SET_BIT(hw->apb1_enr, BIT(periph));
}

/**
 * @brief Enable periphal clock on APB2 bus
 * 
 * @param hw pointer to RCC address
 * @param periph peripheral number in RCC_APB2ENR register
 */
static inline void rcc_ll_apb2_en_clk(rcc_dev_t hw, uint32_t periph)
{
    REG_SET_BIT(hw->apb2_enr, BIT(periph));
}

#endif