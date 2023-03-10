#ifndef __USART_LL_H__

#define __USART_LL_H__

#include "usart_periph.h"

#include "bit_math.h"

#include <stdint.h>
#include <stddef.h>

typedef enum
{
    USART_LL_OVERSAMPLING_BY_16,
    USART_LL_OVERSAMPLING_BY_8
} usart_ll_oversamling_t;

typedef enum
{
    USART_LL_DATA_BITS_8,
    USART_LL_DATA_BITS_9
} usart_ll_data_bits_t;

typedef enum
{
    USART_LL_PARITY_NONE = 0,
    USART_LL_PARITY_EVEN = USART_CR1_PCE_S,
    USART_LL_PARITY_ODD = USART_CR1_PCE_S | USART_CR1_PS_S
} usart_ll_parity_t;

typedef enum
{
    USART_LL_STOP_BITS_1 = 0,
    USART_LL_STOP_BITS_2 = 2
} usart_ll_stop_bits_t;

typedef enum
{
    USART_LL_SAMPLE_3,
    USART_LL_SAMPLE_1
} usart_ll_sample_method_t;

/**
 * @brief Check if TDR is empty 
 * 
 * @param hw 
 * @return uint32_t 1: Empty, 0: Full
 */
static inline uint32_t usart_ll_is_tx_empty(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_TXE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_tx_complete(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_TC_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_tc_flag(usart_dev_t hw)
{
    (void) hw->sr;
    hw->dr = 0x00;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_rx_full(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_RXNE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @note Recommended only for multibuffer transfer
 * @param hw 
 */
static inline void usart_ll_clear_receive_flag(usart_dev_t hw)
{
    REG_CLR_BIT(hw->sr, USART_SR_RXNE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_idle_detected(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_IDLE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_idle_flag(usart_dev_t hw)
{
    (void) hw->sr;
    (void) hw->dr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_overrun_error(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_ORE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_overrun_error(usart_dev_t hw)
{
    (void) hw->sr;  /* Dummy read of USAR_SR */
    (void) hw->dr;  /* Dummy read of USART_DR */
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_noise_error(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_NF_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_noise_error(usart_dev_t hw)
{
    (void) hw->sr;
    (void) hw->dr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_framing_error(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_FE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_framing_error(usart_dev_t hw)
{
    (void) hw->sr;
    (void) hw->dr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_parity_error(usart_dev_t hw)
{
    return REG_GET_BIT(hw->sr, USART_SR_PE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_clear_parity_error(usart_dev_t hw)
{
    (void) hw->sr;
    (void) hw->dr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param data 
 */
static inline void usart_ll_transmit(usart_dev_t hw, uint16_t data)
{
    hw->dr = data;
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint16_t 
 */
static inline uint16_t usart_ll_receive(usart_dev_t hw)
{
    return hw->dr;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param mode 
 */
static inline void usart_ll_set_oversampling_mode(usart_dev_t hw, usart_ll_oversamling_t mode)
{
    if(mode == USART_LL_OVERSAMPLING_BY_8)
    {
        ATOMIC_SET_BIT(hw->cr1, USART_CR1_OVER8_S);
    }
    else
    {
        ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_OVER8_S);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @return usart_ll_oversamling_t 
 */
static inline usart_ll_oversamling_t usart_ll_get_oversamling_mode(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_OVER8_S) ? 
                            USART_LL_OVERSAMPLING_BY_8 : USART_LL_OVERSAMPLING_BY_16;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_UE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_UE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param bits 
 */
static inline void usart_ll_set_data_bits(usart_dev_t hw, usart_ll_data_bits_t bits)
{
    if(bits == USART_LL_DATA_BITS_9)
    {
        ATOMIC_SET_BIT(hw->cr1, USART_CR1_M_S);
    }
    else
    {
        ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_M_S);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @return usart_ll_data_bits_t 
 */
static inline usart_ll_data_bits_t usart_ll_get_data_bits(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_M_S) ? USART_LL_DATA_BITS_9 : USART_LL_DATA_BITS_8;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param parity 
 */
static inline void usart_ll_set_parity_mode(usart_dev_t hw, usart_ll_parity_t parity)
{
    switch(parity)
    {
        case USART_LL_PARITY_NONE:
            ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_PCE_S);
            break;
        case USART_LL_PARITY_EVEN:
            ATOMIC_SET_BIT(hw->cr1, USART_CR1_PCE_S);
            ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_PS_S);
            break;
        case USART_LL_PARITY_ODD:
            ATOMIC_SET_BIT(hw->cr1, USART_CR1_PCE_S | USART_CR1_PS_S);
            break;
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @return usart_ll_parity_t 
 */
static inline usart_ll_parity_t usart_ll_get_parity_mode(usart_dev_t hw)
{
    return (usart_ll_parity_t) 
                (REG_GET_BIT(hw->cr1, USART_CR1_PCE_S) | REG_GET_BIT(hw->cr1, USART_CR1_PS_S));
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_parity_interrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_PEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_parity_interrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_PEIE_S);
}

/**
 * @brief 
 * 
 * @param hw
 * 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_parity_interrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_PEIE_S) ? 1 : 0;
}

static inline void usart_ll_enable_transmit_interrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_TXEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_transmit_interrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_TXEIE_S);
}

/**
 * @brief 
 * 
 * @param hw
 * 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_transmit_interrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_TXEIE_S) ? 1 : 0;
}

static inline void usart_ll_enable_tc_interrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_TCIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_tc_interrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_TCIE_S);
}

/**
 * @brief 
 * 
 * @param hw
 * 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_tc_interrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_TCIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_receive_interrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_RXNIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_receive_interrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_RXNIE_S);
}

/**
 * @brief 
 * 
 * @param hw
 * 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_receive_interrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_RXNIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_idle_interrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_IDLEIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_idle_interrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_IDLEIE_S);
}

/**
 * @brief 
 * 
 * @param hw
 * 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_idle_interrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr1, USART_CR1_IDLEIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_transmitter(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_TE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_transmitter(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_TE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_receiver(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_RE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_receiver(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr1, USART_CR1_RE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_send_break(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr1, USART_CR1_SBK_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param n_stop 
 */
static inline void usart_ll_set_stop_bits(usart_dev_t hw, usart_ll_stop_bits_t n_stop)
{
    REG_WRITE_BITS(hw->cr2, USART_CR2_STOP_S, USART_CR2_STOP_M, n_stop);
}

/**
 * @brief 
 * 
 * @param hw 
 * @return usart_ll_stop_bits_t 
 */
static inline usart_ll_stop_bits_t usart_ll_get_stop_bits(usart_dev_t hw)
{
    return REG_GET_BITS(hw->cr2, USART_CR2_STOP_S, USART_CR2_STOP_M);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param m 
 */
static inline void usart_ll_set_sample_method(usart_dev_t hw, usart_ll_sample_method_t m)
{
    if(m == USART_LL_SAMPLE_1)
    {
        ATOMIC_SET_BIT(hw->cr3, USART_CR3_ONEBIT_S);
    }
    else
    {
        ATOMIC_CLEAR_BIT(hw->cr3, USART_CR3_ONEBIT_S);
    }
}

/**
 * @brief 
 * 
 * @param hw 
 * @return usart_ll_sample_method_t 
 */
static inline usart_ll_sample_method_t usart_ll_get_sample_method(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr3, USART_CR3_ONEBIT_S) ? USART_LL_SAMPLE_1 : USART_LL_SAMPLE_3;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_tx_dma(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr3, USART_CR3_DMAT_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_tx_dma(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr3, USART_CR3_DMAT_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_tx_dma_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr3, USART_CR3_DMAT_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_rx_dma(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr3, USART_CR3_DMAR_S);
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_rx_dma(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr3, USART_CR3_DMAR_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t 
 */
static inline uint32_t usart_ll_is_rx_dma_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr3, USART_CR3_DMAR_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_enable_error_intterrupt(usart_dev_t hw)
{
    ATOMIC_SET_BIT(hw->cr3, USART_CR3_EIE_S);
}
/**
 * @brief 
 * 
 * @param hw 
 */
static inline void usart_ll_disable_error_intterrupt(usart_dev_t hw)
{
    ATOMIC_CLEAR_BIT(hw->cr3, USART_CR3_EIE_S);
}

/**
 * @brief 
 * 
 * @param hw 
 * @return uint32_t
 */
static inline uint32_t usart_ll_error_intterrupt_enabled(usart_dev_t hw)
{
    return REG_GET_BIT(hw->cr3, USART_CR3_EIE_S) ? 1 : 0;
}

/**
 * @brief 
 * 
 * @param hw 
 * @param value 
 */
static inline void usart_ll_brr_set_mantissa(usart_dev_t hw, uint16_t value)
{
    REG_WRITE_BITS(hw->brr, USART_BRR_MANTISSA_S, USART_BRR_MANTISSA_M, value);
}

/**
 * @brief 
 * 
 * @param hw 
 * @param value 
 */
static inline void usart_ll_brr_set_fraction(usart_dev_t hw, uint8_t value)
{
    REG_WRITE_BITS(hw->brr, USART_BRR_FRACTION_S, USART_BRR_FRACTION_M, value);
}

#endif