#ifndef __FLASH_PERIPH_H__

#define __FLASH_PERIPH_H__

#include "bit_math.h"

#include <stdint.h>

#define FLASH_ACR_LATENCY_S     (0)
#define FLASH_ACR_LATENCY_M     (0xF)
#define FLASH_ACR_PRFTEN_S      (BIT(8))
#define FLASH_ACR_ICEN_S        (BIT(9))
#define FLASH_ACR_DCEN_S        (BIT(10))
#define FLASH_ACR_ICRST_S       (BIT(11))
#define FLASH_ACR_DCRST_S       (BIT(12))

#define FLASH_CR_PG_S          (BIT(0))
#define FLASH_CR_SER_S          (BIT(1))
#define FLASH_CR_MER_S          (BIT(2))
#define FLASH_CR_SNB_S          (3)
#define FLASH_CR_SNB_M          (0xF)
#define FLASH_CR_PSIZE_S        (8)
#define FLASH_CR_PSIZE_M        (0x3)
#define FLASH_CR_STRT_S         (BIT(16))
#define FLASH_CR_LOCK_S         (BIT(31))

#define FLASH_SR_BUSY_S         (BIT(16))

#define _FLASH_DEV ((flash_dev_t) 0x40023C00U)

typedef struct
{
    uint32_t acr;
    uint32_t keyr;
    uint32_t optkeyr;
    uint32_t sr;
    uint32_t cr;
    uint32_t optcr;
} volatile * flash_dev_t;


#define FLASH_LL_GET_HW()  ((_FLASH_DEV))

#endif