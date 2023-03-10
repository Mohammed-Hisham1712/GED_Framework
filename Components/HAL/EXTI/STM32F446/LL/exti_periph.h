#ifndef __EXTI_PERIPH_H__

#define __EXTI_PERIPH_H__
/**
 * @brief External Intterupt peripheral register definitions
 * 
 * @author Mohammed Hisham
 * @date December 27, 2022
 */

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>


#define EXTI_BIT(n)    BIT(n)

typedef struct
{
    uint32_t imr;
    uint32_t emr;
    uint32_t rtsr;
    uint32_t ftsr;
    uint32_t swier;
    uint32_t pr;

} volatile * exti_dev_t;

#define _EXTI_DEV   ((exti_dev_t) (0x40013C00))

#define EXTI_LL_GET_HW(num)    ((num) == 0 ? (_EXTI_DEV) : NULL)

#endif
