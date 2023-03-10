#ifndef _RCC_PERIPH_H_

#define _RCC_PERIPH_H_

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

typedef struct 
{
    uint32_t cr;
    uint32_t pll_cfgr;
    uint32_t cfgr;
    uint32_t cir;
    uint32_t ahb1_rstr;
    uint32_t ahb2_rstr;
    uint32_t ahb3_rstr;
    uint32_t reserved_1c;
    uint32_t apb1_rstr;
    uint32_t apb2_rstr;
    uint32_t reserved_28[2];
    uint32_t ahb1_enr;
    uint32_t ahb2_enr;
    uint32_t ahb3_enr;
    uint32_t reserved_3c;
    uint32_t apb1_enr;
    uint32_t apb2_enr;
    uint32_t reserved_48[2];
    /*
    * TODO implement the rest of registers
    */
} volatile* rcc_dev_t;

#define _RCC_DEV    ((rcc_dev_t) (0x40023800))

#define RCC_LL_GET_HW(num)  ((num == 0) ? _RCC_DEV : NULL)


#endif