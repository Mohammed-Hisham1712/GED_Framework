#ifndef __SYSCFG_PERIPH_H__

#define __SYSCFG_PERIPH_H__

/**
 * @brief System Configuration peripheral
 * Used to configure
 * 
 * 1- EXTI connection to GPIO ports
 * 2-
 * 
 * @note Don't include this file in your Application
 * 
 * @author Mohammed Hisham
 * @date Dec. 27, 2022
 */

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

#define SYSCFG_EXTICR_REG(hw, _exti_num)    (hw->exticr + (_exti_num >> 2))

#define SYSCFG_EXTICR_S(_exti_num)          (((_exti_num) & 0x03) << 2)
#define SYSCFG_EXTICR_M                     0x0FUL

typedef struct
{
    uint32_t memrmp;
    uint32_t pmc;
    uint32_t exticr[4];
    uint32_t cmpcr;
    uint32_t cfgr;

} volatile* syscfg_dev_t;

#define _SYSCFG_DEV     ((syscfg_dev_t) (0x40013800))

#define SYSCFG_LL_GET_HW(num)  ((num) == 0 ? (_SYSCFG_DEV) : NULL)

#endif