#ifndef __TIM_BASIC_HAL_H__

#define __TIM_BASIC_HAL_H__

#include "TIM_BASIC_types.h"
#include "TIM_BASIC_periph.h"

#include "types.h"

#define TIM_BASIC_HAL_GET_HW(hw, tim)   ((hw)->dev = TIM_BASIC_LL_GET_HW(tim))


/**
 * @brief 
 * 
 * @param tim 
 * @param config 
 * @return tim_basic_hal_context_t* 
 */
tim_basic_hal_context_t* tim_basic_hal_init(uint8_t tim, tim_basic_config_t* config);


/**
 * @brief 
 * 
 * @param p_timer 
 * @return error_t 
 */
error_t tim_basic_hal_start(tim_basic_hal_context_t* p_timer);

/**
 * @brief 
 * 
 * @param p_timer 
 * @return error_t 
 */
error_t tim_basic_hal_stop(tim_basic_hal_context_t* p_timer);


#endif