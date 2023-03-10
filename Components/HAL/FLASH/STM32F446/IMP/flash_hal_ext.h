#ifndef __FLASH_HAL_EXT_H__

#define __FLASH_HAL_EXT_H__

#include "flash_ll.h"
#include "flash_hal.h"
#include "flash_types.h"

#define FLASH_HAL_GET_HW(context)       ((context)->dev = FLASH_LL_GET_HW())

#include <stdint.h>

#endif