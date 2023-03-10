#ifndef __FLASH_LL_H__

#define __FLASH_LL_H__

#include "flash_periph.h"
#include "bit_math.h"
#include "types.h"

#include <stddef.h>
#include <stdint.h>

#define FLASH_LL_UNLOCK_KEY1    (0x45670123)
#define FLASH_LL_UNLOCK_KEY2    (0xCDEF89AB)

typedef enum
{
    FLASH_LL_WAIT_STATES_0 = 0,
    FLASH_LL_WAIT_STATES_1,
    FLASH_LL_WAIT_STATES_2,
    FLASH_LL_WAIT_STATES_3,
    FLASH_LL_WAIT_STATES_4,
    FLASH_LL_WAIT_STATES_5,
    FLASH_LL_WAIT_STATES_6,
    FLASH_LL_WAIT_STATES_7,
    FLASH_LL_WAIT_STATES_8,
    FLASH_LL_WAIT_STATES_9,
    FLASH_LL_WAIT_STATES_10,
    FLASH_LL_WAIT_STATES_11,
    FLASH_LL_WAIT_STATES_12,
    FLASH_LL_WAIT_STATES_13,
    FLASH_LL_WAIT_STATES_14,
    FLASH_LL_WAIT_STATES_15,
} flash_ll_wait_states_t;

typedef enum
{
    FLASH_LL_PSIZE_8 = 0,
    FLASH_LL_PSIZE_16,
    FLASH_LL_PSIZE_32,
    FLASH_LL_PSIZE_64,
} flash_ll_psize_t;

typedef enum
{
    FLASH_LL_SECTOR_0 = 0,
    FLASH_LL_SECTOR_1,
    FLASH_LL_SECTOR_2,
    FLASH_LL_SECTOR_3,
    FLASH_LL_SECTOR_4,
    FLASH_LL_SECTOR_5,
    FLASH_LL_SECTOR_6,
    FLASH_LL_SECTOR_7,
} flash_ll_sec_num_t;

STATIC_INLINE void flash_ll_enable_icache(flash_dev_t dev)
{
    REG_SET_BIT(dev->acr, FLASH_ACR_ICEN_S);
}

STATIC_INLINE void flash_ll_disable_icache(flash_dev_t dev)
{
    REG_CLR_BIT(dev->acr, FLASH_ACR_ICEN_S);
}

STATIC_INLINE uint32_t flash_ll_icache_enabled(flash_dev_t dev)
{
    return REG_GET_BIT(dev->acr, FLASH_ACR_ICEN_S) ? 1 : 0;
}

STATIC_INLINE void flash_ll_enable_dcache(flash_dev_t dev)
{
    REG_SET_BIT(dev->acr, FLASH_ACR_DCEN_S);
}

STATIC_INLINE void flash_ll_disable_dcache(flash_dev_t dev)
{
    REG_CLR_BIT(dev->acr, FLASH_ACR_DCEN_S);
}

STATIC_INLINE uint32_t flash_ll_dcache_enabled(flash_dev_t dev)
{
    return REG_GET_BIT(dev->acr, FLASH_ACR_DCEN_S) ? 1 : 0;
}

STATIC_INLINE void flash_ll_flush_dcache(flash_dev_t dev)
{
    REG_CLR_BIT(dev->acr, FLASH_ACR_DCEN_S);
    REG_SET_BIT(dev->acr, FLASH_ACR_DCRST_S);
    REG_CLR_BIT(dev->acr, FLASH_ACR_DCRST_S);
    REG_SET_BIT(dev->acr, FLASH_ACR_DCEN_S);
}

STATIC_INLINE void flash_ll_flush_icache(flash_dev_t dev)
{
    REG_CLR_BIT(dev->acr, FLASH_ACR_ICEN_S);
    REG_SET_BIT(dev->acr, FLASH_ACR_ICRST_S);
    REG_CLR_BIT(dev->acr, FLASH_ACR_ICRST_S);
    REG_SET_BIT(dev->acr, FLASH_ACR_ICEN_S);
}

STATIC_INLINE void flash_ll_enable_prefetch(flash_dev_t dev)
{
    REG_SET_BIT(dev->acr, FLASH_ACR_PRFTEN_S);
}

STATIC_INLINE void flash_ll_disable_prefetch(flash_dev_t dev)
{
    REG_CLR_BIT(dev->acr, FLASH_ACR_PRFTEN_S);
}

STATIC_INLINE uint32_t flash_ll_prefetch_enabled(flash_dev_t dev)
{
    return REG_GET_BIT(dev->acr, FLASH_ACR_PRFTEN_S) ? 1 : 0;
}

STATIC_INLINE void flash_ll_set_wait_states(flash_dev_t dev, flash_ll_wait_states_t ws)
{
    REG_WRITE_BITS(dev->acr, FLASH_ACR_LATENCY_S, FLASH_ACR_LATENCY_M, ws);
}

STATIC_INLINE uint32_t flash_ll_get_wait_states(flash_dev_t dev)
{
    return REG_GET_BITS(dev->acr, FLASH_ACR_LATENCY_S, FLASH_ACR_LATENCY_M);
}

STATIC_INLINE void flash_ll_unlock(flash_dev_t dev)
{
    dev->keyr = FLASH_LL_UNLOCK_KEY1;
    dev->keyr = FLASH_LL_UNLOCK_KEY2;
}

STATIC_INLINE void flash_ll_lock(flash_dev_t dev)
{
    REG_SET_BIT(dev->cr, FLASH_CR_LOCK_S);
}

STATIC_INLINE uint32_t flash_ll_is_locked(flash_dev_t dev)
{
    return REG_GET_BIT(dev->cr, FLASH_CR_LOCK_S) ? 1 : 0;
}

STATIC_INLINE void flash_ll_start(flash_dev_t dev)
{
    REG_SET_BIT(dev->cr, FLASH_CR_STRT_S);
}

STATIC_INLINE void flash_ll_set_psize(flash_dev_t dev, flash_ll_psize_t size)
{
    REG_WRITE_BITS(dev->cr, FLASH_CR_PSIZE_S, FLASH_CR_PSIZE_M, size);
}

STATIC_INLINE uint32_t flash_ll_get_psize(flash_dev_t dev)
{
    return REG_GET_BITS(dev->cr, FLASH_CR_PSIZE_S, FLASH_CR_PSIZE_M);
}

STATIC_INLINE void flash_ll_set_sector_num(flash_dev_t dev, flash_ll_sec_num_t sector)
{
    REG_WRITE_BITS(dev->cr, FLASH_CR_SNB_S, FLASH_CR_SNB_M, sector);
}

STATIC_INLINE uint32_t flash_ll_get_sector_num(flash_dev_t dev)
{
    return REG_GET_BITS(dev->cr, FLASH_CR_SNB_S, FLASH_CR_SNB_M);
}

STATIC_INLINE void flash_ll_enable_mass_erase(flash_dev_t dev)
{
    REG_CLR_BIT(dev->cr, FLASH_CR_MER_S | FLASH_CR_SER_S | FLASH_CR_PG_S);
    REG_SET_BIT(dev->cr, FLASH_CR_MER_S);
}

STATIC_INLINE void flash_ll_enable_sector_erase(flash_dev_t dev)
{
    REG_CLR_BIT(dev->cr, FLASH_CR_MER_S | FLASH_CR_SER_S | FLASH_CR_PG_S);
    REG_SET_BIT(dev->cr, FLASH_CR_SER_S);
}

STATIC_INLINE void flash_ll_enable_programming(flash_dev_t dev)
{
    REG_CLR_BIT(dev->cr, FLASH_CR_MER_S | FLASH_CR_SER_S | FLASH_CR_PG_S);
    REG_SET_BIT(dev->cr, FLASH_CR_PG_S);
}

STATIC_INLINE void flash_ll_disable_programming(flash_dev_t dev)
{
    REG_CLR_BIT(dev->cr, FLASH_CR_PG_S);
}

STATIC_INLINE uint32_t flash_ll_is_busy(flash_dev_t dev)
{
    return REG_GET_BIT(dev->sr, FLASH_SR_BUSY_S) ? 1 : 0;
}

#endif