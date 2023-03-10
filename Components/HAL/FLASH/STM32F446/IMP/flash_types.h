#ifndef __FLASH_TYPES_H__

#define __FLASH_TYPES_H__

#include "flash_periph.h"

enum
{
    FLASH_VOLTAGE_RANGE_1 = 0U, /* Voltage range 1.7-2.1 V */
    FLASH_VOLTAGE_RANGE_2,      /* Voltage range 2.1-2.7 V */
    FLASH_VOLTAGE_RANGE_3,      /* Voltage range 2.7-3.6 V */
    FLASH_VOLTAGE_RANGE_4,      /* Voltage range 2.7-3.6 V with External Vpp*/
    FLASH_VOLTAGE_RANGE_INV
};

enum
{
    FLASH_SECTOR_0 = 0U,
    FLASH_SECTOR_1,
    FLASH_SECTOR_2,
    FLASH_SECTOR_3,
    FLASH_SECTOR_4,
    FLASH_SECTOR_5,
    FLASH_SECTOR_6,
    FLASH_SECTOR_7,
    FLASH_SECTOR_INV,
};

enum
{
    FLASH_PROGRAM_BYTE,
    FLASH_PROGRAM_HALFWORD,
    FLASH_PROGRAM_WORD,
    FLASH_PROGRAM_DOUBLEWORD,
};

typedef struct 
{
    flash_dev_t dev;
} flash_hal_context_t;

#endif