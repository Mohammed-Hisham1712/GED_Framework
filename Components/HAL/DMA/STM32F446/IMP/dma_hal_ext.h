#ifndef __DMA_HAL_EXT_H__

#define __DMA_HAL_EXT_H__

#include "dma_ll.h"
#include "dma_types.h"

#include "types.h"

#include <stdint.h>
#include <stddef.h>

#define DMA_HAL_GET_HW(context, num)    ((context)->dev = DMA_LL_GET_HW((num)))

/**
 * @brief 
 * 
 * @param dma_instance 
 * @param stream 
 * @return dma_hal_context_t* 
 */
dma_hal_context_t* dma_hal_init(uint8_t dma_instance, uint8_t stream);

/**
 * @brief Init DMA and set configuration parameters
 * 
 * @param dma
 * @return error_t 
 */
error_t dma_hal_stream_init(dma_hal_context_t* dma);

/**
 * @brief Set transfer parameters in Single Buffer Mode
 * 
 * @param dma 
 * @param src_addr 
 * @param dest_addr 
 * @param len 
 * @return error_t 
 */
error_t dma_hal_set_transfer(dma_hal_context_t* dma, 
            uint32_t src_addr, uint32_t dest_addr, uint16_t len);

/**
 * @brief Start DMA transfer in polling mode
 * 
 * @param dma 
 * @return error_t 
 */
error_t dma_hal_start(dma_hal_context_t* dma);

/**
 * @brief 
 * 
 * @param dma 
 * @return error_t 
 */
error_t dma_hal_start_it(dma_hal_context_t* dma);

/**
 * @brief 
 * 
 * @param dma 
 * @param level
 * @return error_t 
 */
error_t dma_hal_poll_for_transfer(dma_hal_context_t* dma, uint8_t level);

/**
 * @brief 
 * 
 * @param dma 
 * @return error_t 
 */
error_t dma_hal_abort(dma_hal_context_t* dma);

/**
 * @brief 
 * 
 * @param dma 
 * @return error_t 
 */
error_t dma_hal_abort_from_isr(dma_hal_context_t* dma);

/**
 * @brief 
 * 
 * @param dma 
 * @return uint16_t 
 */
uint16_t dma_hal_get_remaining_items(dma_hal_context_t* dma);

#endif