#ifndef __DMA_LL_H__

#define __DMA_LL_H__

#include "dma_periph.h"

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>


typedef enum
{
    DMA_LL_CT_MEM0,
    DMA_LL_CT_MEM1
} dma_ll_target_t;

typedef enum
{
    DMA_LL_PRI_LOW,
    DMA_LL_PRI_MEDIUM,
    DMA_LL_PRI_HIGH,
    DMA_LL_PRI_VERY_HIGH
} dma_ll_priority_t;

typedef enum
{
    DMA_LL_SIZE_BYTE,
    DMA_LL_SIZE_HALF_WORD,
    DMA_LL_SIZE_WORD,
} dma_ll_size_t;

typedef enum
{
    DMA_LL_DIR_PERIPH_MEM,
    DMA_LL_DIR_MEM_PERIPH,
    DMA_LL_DIR_MEM_MEM
} dma_ll_dir_t;

static const uint8_t stream_indx[] = {0, 6, 16, 22};

/**
 * @brief 
 * 
 * @param hw Pointer to start address of DMA
 * @param stream One of the available dma streams
 * @param channel The channel to configure for this stream
 * 
 * @note write protected. EN must be 0
 */
static inline void dma_ll_set_channel(dma_dev_t hw, uint8_t stream, uint8_t channel)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_CHSEL_S, DMA_SxCR_CHSEL_M, channel);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_channel(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_CHSEL_S, DMA_SxCR_CHSEL_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param burst_size 
 * 
 * @note write protected. EN must be 0
 */
static inline void dma_ll_set_mem_burst(dma_dev_t hw, uint8_t stream, uint8_t burst_size)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_MBURST_S, DMA_SxCR_MBURST_M, burst_size);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_mem_burst(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_MBURST_S, DMA_SxCR_MBURST_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param burst_size 
 * 
 * @note write protected. EN must be 0
 */
static inline void dma_ll_set_periph_burst(dma_dev_t hw, uint8_t stream, uint8_t burst_size)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_PBURST_S, DMA_SxCR_PBURST_M, burst_size);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_periph_burst(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_PBURST_S, DMA_SxCR_PBURST_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param ct 
 */
static inline void dma_ll_set_current_target(dma_dev_t hw, uint8_t stream, dma_ll_target_t ct)
{
    if(ct == DMA_LL_CT_MEM0)
    {
        REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_CT_S);
    }
    else
    {
        REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_CT_S);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return dma_ll_target_t 
 */
static inline dma_ll_target_t dma_ll_get_current_target(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_CT_S) ? DMA_LL_CT_MEM1 : DMA_LL_CT_MEM0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_double_buffer(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_DBM_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_double_buffer(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_DBM_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_double_buffer(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_DBM_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param pri 
 */
static inline void dma_ll_set_priority(dma_dev_t hw, uint8_t stream, dma_ll_priority_t pri)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_PL_S, DMA_SxCR_PL_M, pri);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return dma_ll_priority_t 
 */
static inline dma_ll_priority_t dma_ll_get_priority(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_PL_S, DMA_SxCR_PL_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param sz 
 */
static inline void dma_ll_set_mem_transfer_size(dma_dev_t hw, uint8_t stream, dma_ll_size_t sz)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_MSIZE_S, DMA_SxCR_MSIZE_M, sz);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return dma_ll_size_t 
 */
static inline dma_ll_size_t dma_ll_get_mem_transfer_size(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_MSIZE_S, DMA_SxCR_MSIZE_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param sz 
 */
static inline void dma_ll_set_periph_transfer_size(dma_dev_t hw, uint8_t stream,
                                                                 dma_ll_size_t sz)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_PSIZE_S, DMA_SxCR_PSIZE_M, sz);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return dma_ll_size_t 
 */
static inline dma_ll_size_t dma_ll_get_periph_transfer_size(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_PSIZE_S, DMA_SxCR_PSIZE_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_mem_inc(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_MINC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_mem_inc(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_MINC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_mem_inc(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_MINC_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_periph_inc(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_PINC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_periph_inc(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_PINC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_periph_inc(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_PINC_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_circular_mode(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_CIRC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_circular_mode(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_CIRC_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint8_t 
 */
static inline uint8_t dma_ll_get_circular_mode(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_CIRC_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param dir 
 */
static inline void dma_ll_set_direction(dma_dev_t hw, uint8_t stream, dma_ll_dir_t dir)
{
    REG_WRITE_BITS(SxCR(hw, stream), DMA_SxCR_DIR_S, DMA_SxCR_DIR_M, dir);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return dma_ll_dir_t 
 */
static inline dma_ll_dir_t dma_ll_get_direction(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BITS(SxCR(hw, stream), DMA_SxCR_DIR_S, DMA_SxCR_DIR_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param num 
 */
static inline void dma_ll_set_number_of_transfers(dma_dev_t hw, uint8_t stream, uint16_t num)
{
    SxNDTR(hw, stream) = num;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint16_t 
 */
static inline uint16_t dma_ll_get_remaining_items(dma_dev_t hw, uint8_t stream)
{
    return SxNDTR(hw, stream) & 0xFFFF;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param addr 
 */
static inline void dma_ll_set_periph_addr(dma_dev_t hw, uint8_t stream, uint32_t addr)
{
    SxPAR(hw, stream) = addr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_periph_addr(dma_dev_t hw, uint8_t stream)
{
    return SxPAR(hw, stream);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param addr 
 */
static inline void dma_ll_set_mem_addr_0(dma_dev_t hw, uint8_t stream, uint32_t addr)
{
    SxMAR0(hw, stream) = addr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_mem_addr_0(dma_dev_t hw, uint8_t stream)
{
    return SxMAR0(hw, stream);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @param addr 
 */
static inline void dma_ll_set_mem_addr_1(dma_dev_t hw, uint8_t stream, uint32_t addr)
{
    SxMAR1(hw, stream) = addr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_mem_addr_1(dma_dev_t hw, uint8_t stream)
{
    return SxMAR1(hw, stream);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_direct_mode(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxFCR(hw, stream), DMA_SxFCR_DMDIS_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_direct_mode(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxFCR(hw, stream), DMA_SxFCR_DMDIS_S);
}

/**
 * @brief Construct a new dma ll enable stream object
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_stream(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_EN_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_stream(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_EN_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_is_stream_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_EN_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_transfer_complete_flag(dma_dev_t hw, uint8_t stream)
{
    uint32_t temp;

    if(stream < 4)
    {
        temp = REG_GET_BIT(hw->lisr, DMA_ISR_TCI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        temp = REG_GET_BIT(hw->hisr, DMA_ISR_TCI_S << stream_indx[stream]);
    }

    return temp ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_half_transfer_flag(dma_dev_t hw, uint8_t stream)
{
    uint32_t temp;

    if(stream < 4)
    {
        temp = REG_GET_BIT(hw->lisr, DMA_ISR_HTI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        temp = REG_GET_BIT(hw->hisr, DMA_ISR_HTI_S << stream_indx[stream]);
    }

    return temp ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_transfer_error_flag(dma_dev_t hw, uint8_t stream)
{
    uint32_t temp;

    if(stream < 4)
    {
        temp = REG_GET_BIT(hw->lisr, DMA_ISR_TEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        temp = REG_GET_BIT(hw->hisr, DMA_ISR_TEI_S << stream_indx[stream]);
    }

    return temp ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_direct_mode_error_flag(dma_dev_t hw, uint8_t stream)
{
    uint32_t temp;

    if(stream < 4)
    {
        temp = REG_GET_BIT(hw->lisr, DMA_ISR_DMEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        temp = REG_GET_BIT(hw->hisr, DMA_ISR_DMEI_S << stream_indx[stream]);
    }

    return temp ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 * @return uint32_t 
 */
static inline uint32_t dma_ll_get_fifo_error_flag(dma_dev_t hw, uint8_t stream)
{
    uint32_t temp;

    if(stream < 4)
    {
        temp = REG_GET_BIT(hw->lisr, DMA_ISR_FEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        temp = REG_GET_BIT(hw->hisr, DMA_ISR_FEI_S << stream_indx[stream]);
    }

    return temp ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_transfer_complete_flag(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        REG_SET_BIT(hw->lifcr, DMA_ISR_TCI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        REG_SET_BIT(hw->hifcr, DMA_ISR_TCI_S << stream_indx[stream]);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_half_transfer_flag(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        REG_SET_BIT(hw->lifcr, DMA_ISR_HTI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        REG_SET_BIT(hw->hifcr, DMA_ISR_HTI_S << stream_indx[stream]);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_transfer_error_flag(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        REG_SET_BIT(hw->lifcr, DMA_ISR_TEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        REG_SET_BIT(hw->hifcr, DMA_ISR_TEI_S << stream_indx[stream]);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_direct_mode_error_flag(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        REG_SET_BIT(hw->lifcr, DMA_ISR_DMEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        REG_SET_BIT(hw->hifcr, DMA_ISR_DMEI_S << stream_indx[stream]);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_fifo_error_flag(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        REG_SET_BIT(hw->lifcr, DMA_ISR_FEI_S << stream_indx[stream]);
    }
    else
    {
        stream -= 4;
        REG_SET_BIT(hw->hifcr, DMA_ISR_FEI_S << stream_indx[stream]);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_clear_flags(dma_dev_t hw, uint8_t stream)
{
    if(stream < 4)
    {
        hw->lifcr = DMA_ISR_ALL_S << stream_indx[stream];
    }
    else
    {
        stream -= 4;
        hw->hifcr = DMA_ISR_ALL_S << stream_indx[stream];
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_transfer_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_TEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_direct_mode_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_DMEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_fifo_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxFCR(hw, stream), DMA_SxFCR_FEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_half_transfer_it(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_HTIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_enable_complete_transfer_it(dma_dev_t hw, uint8_t stream)
{
    REG_SET_BIT(SxCR(hw, stream), DMA_SxCR_TCIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_transfer_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_TEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_direct_mode_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_DMEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_fifo_error_it(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxFCR(hw, stream), DMA_SxFCR_FEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_half_transfer_it(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_HTIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline void dma_ll_disable_complete_transfer_it(dma_dev_t hw, uint8_t stream)
{
    REG_CLR_BIT(SxCR(hw, stream), DMA_SxCR_TCIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline uint32_t dma_ll_is_transfer_error_it_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_TEIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline uint32_t dma_ll_is_direct_mode_error_it_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_DMEIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline uint32_t dma_ll_is_transfer_complete_it_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_TCIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline uint32_t dma_ll_is_half_transfer_it_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxCR(hw, stream), DMA_SxCR_HTIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param stream 
 */
static inline uint32_t dma_ll_is_fifo_error_it_enabled(dma_dev_t hw, uint8_t stream)
{
    return REG_GET_BIT(SxFCR(hw, stream), DMA_SxFCR_FEIE_S) ? 1 : 0;
}

#endif