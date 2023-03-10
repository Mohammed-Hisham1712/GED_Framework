#ifndef __USART_HAL_EXT_H__

#define __USART_HAL_EXT_H__

#include "usart_ll.h"
#include "usart_types.h"
#include "usart_hal.h"

#define USART_HAL_GET_HW(context, num)      ((context)->dev = USART_LL_GET_HW((num)))

#endif