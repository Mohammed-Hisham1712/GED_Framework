#include "flash_hal_ext.h"
#include "assert.h"

#include <stdint.h>
#include <stddef.h>

#define FLASH_TIMEOUT       5000U

#define FLASH_IS_VOLTAGE_RANGE(vrange)      ((vrange) < FLASH_VOLTAGE_RANGE_INV)
#define FLASH_IS_SECTOR(sector)             ((sector) < FLASH_SECTOR_INV)
#define FLASH_IS_PROGRAM_TYPE(type)         ((type) <= FLASH_PROGRAM_DOUBLEWORD)

#define FLASH_IS_ADDRESS(addr)              (((addr) >= 0x08000000U) && ((addr) <= 0x0807FFFFU))

/* Private global variables */
static flash_hal_context_t flash;

/* Private functions declaration */
static error_t flash_hal_sector_erase(uint8_t sector, uint8_t voltage_range);
static error_t flash_hal_mass_erase(uint8_t voltage_range);
static error_t flash_hal_wait_for_last_op(uint32_t timeout);
static error_t flash_hal_program_byte(uint32_t address, uint8_t data);
static error_t flash_hal_program_halfword(uint32_t address, uint16_t data);
static error_t flash_hal_program_word(uint32_t address, uint32_t data);
static error_t flash_hal_program_doubleword(uint32_t address, uint64_t data);
static void flash_hal_flush_caches(void);


/* Function definitions */
error_t flash_hal_program(uint32_t data_type, uint32_t address, const void* pdata)
{
    error_t ret = FAILED;

    ASSERT(FLASH_IS_PROGRAM_TYPE(data_type));
    ASSERT(pdata);

    ret = flash_hal_wait_for_last_op(FLASH_TIMEOUT);

    if(ret == OK)
    {
        if(data_type == FLASH_PROGRAM_BYTE)
        {
            flash_hal_program_byte(address, *((const uint8_t*) pdata));
        }
        else if(data_type == FLASH_PROGRAM_HALFWORD)
        {
            flash_hal_program_halfword(address, *((const uint16_t*) pdata));
        }
        else if(data_type == FLASH_PROGRAM_WORD)
        {
            flash_hal_program_word(address, *((const uint32_t*) pdata));
        }
        else
        {
            flash_hal_program_doubleword(address, *((const uint64_t*) pdata));
        }

        ret = flash_hal_wait_for_last_op(FLASH_TIMEOUT);

        flash_ll_disable_programming(flash.dev);
    }

    return ret;
}

static error_t flash_hal_sector_erase(uint8_t sector, uint8_t voltage_range)
{
    ASSERT(FLASH_IS_VOLTAGE_RANGE(voltage_range));
    ASSERT(FLASH_IS_SECTOR(sector));

    if(voltage_range == FLASH_VOLTAGE_RANGE_1)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_8);
    }
    else if(voltage_range == FLASH_VOLTAGE_RANGE_2)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_16);
    }
    else if(voltage_range == FLASH_VOLTAGE_RANGE_3)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_32);
    }
    else
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_64);
    }

    flash_ll_enable_sector_erase(flash.dev);
    flash_ll_set_sector_num(flash.dev, sector);

    flash_ll_start(flash.dev);

    return OK;
}

static error_t flash_hal_mass_erase(uint8_t voltage_range)
{
    ASSERT(FLASH_IS_VOLTAGE_RANGE(voltage_range));

    if(voltage_range == FLASH_VOLTAGE_RANGE_1)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_8);
    }
    else if(voltage_range == FLASH_VOLTAGE_RANGE_2)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_16);
    }
    else if(voltage_range == FLASH_VOLTAGE_RANGE_3)
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_32);
    }
    else
    {
        flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_64);
    }

    flash_ll_enable_mass_erase(flash.dev);    
    flash_ll_start(flash.dev);

    return OK;
}

static error_t flash_hal_wait_for_last_op(uint32_t timeout)
{
    (void) timeout;

    while(flash_ll_is_busy(flash.dev))
    {
        /*
            TODO: Check for timeout
        */
    }

    return OK;
}

static error_t flash_hal_program_byte(uint32_t address, uint8_t data)
{
    ASSERT(FLASH_IS_ADDRESS(address));

    flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_8);
    flash_ll_enable_programming(flash.dev);

    *(uint8_t*) address = data;

    return OK;
}

static error_t flash_hal_program_halfword(uint32_t address, uint16_t data)
{
    ASSERT(FLASH_IS_ADDRESS(address));

    flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_16);
    flash_ll_enable_programming(flash.dev);

    *(uint16_t*) address = data;

    return OK;
}

static error_t flash_hal_program_word(uint32_t address, uint32_t data)
{
    ASSERT(FLASH_IS_ADDRESS(address));

    flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_32);
    flash_ll_enable_programming(flash.dev);

    *(uint32_t*) address = data;

    return OK;
}

static error_t flash_hal_program_doubleword(uint32_t address, uint64_t data)
{
    ASSERT(FLASH_IS_ADDRESS(address));

    flash_ll_set_psize(flash.dev, FLASH_LL_PSIZE_64);
    flash_ll_enable_programming(flash.dev);

    *(uint64_t*) address = data;

    return OK;
}

static void flash_hal_flush_caches(void)
{
    if(flash_ll_icache_enabled(flash.dev))
    {
        /* ICache is enabled */
        flash_ll_flush_icache(flash.dev);
    }

    if(flash_ll_dcache_enabled(flash.dev))
    {
        /* DCache is enabled */
        flash_ll_flush_dcache(flash.dev);
    }
}