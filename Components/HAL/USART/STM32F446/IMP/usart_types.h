
#ifndef __USART_TYPES_H__

#define __USART_TYPES_H__

#include "usart_periph.h"
#include "dma_types.h"

typedef enum
{
    USART_WORDLENGTH_8B,
    USART_WORDLENGTH_9B,
    USART_WORDLENGTH_INV
} usart_word_length_t;

typedef enum
{
    USART_STOPBITS_1 = 0,
    USART_STOPBITS_2 = 2,
    USART_STOPBITS_INV
} usart_stop_bits_t;

typedef enum
{
    USART_PARITY_NONE,
    USART_PARITY_EVEN,
    USART_PARITY_ODD,
    USART_PARITY_INV
} usart_parity_t;

enum
{
    USART1,
    USART2,
    USART3,
    USART4,
    USART5,
    USART6,
    USART_INV
};

#define MODE_TX     0x0001
#define MODE_RX     0x0002

#define USART_MODE_TX       MODE_TX
#define USART_MODE_RX       MODE_RX
#define USART_MODE_TX_RX    (MODE_TX | MODE_RX)

#define USART_ERROR_NONE    0x00000000U
#define USART_ERROR_PE      0x00000001U
#define USART_ERROR_NE      0x00000002U
#define USART_ERROR_FE      0x00000004U
#define USART_ERROR_ORE     0x00000008U
#define USART_ERROR_DMA     0x00000010U

typedef enum
{
    USART_TX_COMPLETE_CALLBACK,
    USART_RX_COMPLETE_CALLBACK,
    USART_RX_HALF_COMPLETE_CALLBACK,
    USART_RX_IDLE_RECEIVED_CALLBACK,
    USART_ERROR_CALLBACK,
    USART_CALLBACK_INV
} usart_callback_id_t;

typedef struct s_usart_hal_context_t
{
    usart_dev_t dev;
    dma_hal_context_t* tx_dma;
    const uint8_t* tx_pbuffer;
    dma_hal_context_t* rx_dma;
    uint8_t* rx_pbuffer;
    uint32_t error_code;
    uint16_t tx_buffersize;
    uint16_t tx_count;
    uint16_t rx_buffersize;
    uint16_t rx_count;
    void (*tx_complete_callback) (struct s_usart_hal_context_t*);
    void (*rx_complete_callback) (struct s_usart_hal_context_t*);
    void (*rx_half_complete_callback) (struct s_usart_hal_context_t*);
    void (*rx_idle_received_callback) (struct s_usart_hal_context_t*);
    void (*error_callback) (struct s_usart_hal_context_t*);
} usart_hal_context_t;

#endif