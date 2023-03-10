#include "usart_hal_ext.h"

#include "rcc_hal_ext.h"
#include "gpio_hal_ext.h"
#include "dma_hal_ext.h"
#include "assert.h"
#include "stm32f446xx.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define USART_IS_INSTANCE(id)       ((id) < USART_INV)
#define USART_IS_WORDLENGTH(wl)     ((wl) < USART_WORDLENGTH_INV)
#define USART_IS_STOPBITS(stpb)     ((stpb) == USART_STOPBITS_1 || (stpb) == USART_STOPBITS_2)
#define USART_IS_PARITY(parity)     ((parity) < USART_PARITY_INV)
#define USART_IS_MODE(mode)         (((mode) == USART_MODE_TX) || ((mode) == USART_MODE_RX) \
                                                || ((mode) == USART_MODE_TX_RX))
#define USART_IS_CALLBACK(_id)      ((_id) < USART_CALLBACK_INV)

/* PRIVATE GLOBAL VARIABLES */

static usart_hal_context_t usartx[USART_LL_DEVS];


/* PRIVATE FUNCTIONS */

static error_t usart_hal_set_brr(usart_hal_context_t* usart, uint32_t pclk, 
                                                            uint32_t baudrate);

static error_t usart_hal_transmit_16b(usart_hal_context_t* usart, 
                                        const uint16_t* pdata, uint16_t size);

static error_t usart_hal_transmit_from_isr(usart_hal_context_t* usart);

static error_t usart_hal_transmit_8b(usart_hal_context_t* usart, 
                                        const uint8_t* pdata, uint16_t size);

static void usart_hal_tx_dma_complete(dma_hal_context_t* dma);
static void usart_hal_rx_dma_complete(dma_hal_context_t* dma);

static void usart_hal_tx_dma_error(dma_hal_context_t* dma);
static void usart_hal_rx_dma_error(dma_hal_context_t* dma);
static void usart_hal_rx_dma_half_complete(dma_hal_context_t* dma);

static error_t usart_hal_receive_polling(usart_hal_context_t* usart, 
                                        uint8_t* pbuffer, uint16_t size, uint8_t toidle);
static error_t usart_hal_receive_start_isr(usart_hal_context_t* usart, 
                                        uint8_t* pbuffer, uint16_t size, uint8_t toidle);
static error_t usart_hal_receive_start_dma(usart_hal_context_t* usart,
                                        uint8_t* pbuffer, uint16_t size, uint8_t toidle);

/**
 * @brief 
 * 
 * @param usart 
 * @return error_t 
 * 
 * @note Analyzing performance @16MHz with no errors detected
 * Exec. time Te = 28uS
 */
static error_t usart_hal_receive_from_isr(usart_hal_context_t* usart);
static error_t usart_hal_multibuffer_error(usart_hal_context_t* usart);

/* PUBLIC FUNCTIONS */

usart_hal_context_t* usart_hal_init(uint8_t usart_id)
{
    rcc_hal_context_t rcc;
    gpio_hal_context_t gpio;
    usart_hal_context_t* usart;

    ASSERT(USART_IS_INSTANCE(usart_id));
    
    usart = &usartx[usart_id];

    memset((void*) usart, 0, sizeof(usart_hal_context_t));
    USART_HAL_GET_HW(usart, usart_id);
    ASSERT(usart->dev);

    RCC_HAL_GET_HW(&rcc, RCC);

    if(usart_id == USART1)
    {
        GPIO_HAL_GET_HW(&gpio, GPIOA);
        gpio_hal_init(GPIOA);
        gpio_hal_set_mode_alternate_pp(&gpio, GPIO_PIN_9, 7);
        gpio_hal_set_mode_alternate_pp(&gpio, GPIO_PIN_10, 7);
        rcc_hal_apb2_en_clk(&rcc, RCC_HAL_USART1);
        NVIC_EnableIRQ(USART1_IRQn);
    }
    else if(usart_id == USART6)
    {
        rcc_hal_apb2_en_clk(&rcc, RCC_HAL_USART6);
    }
    else
    {
        usart = NULL;
    }

    return usart;
}

error_t usart_hal_setup(usart_hal_context_t* usart, uint32_t baudrate,
                    uint8_t wordlength, uint8_t stopbits, uint8_t parity, uint8_t mode)
{
    error_t ret = OK;

    ASSERT(usart);
    ASSERT(USART_IS_WORDLENGTH(wordlength));
    ASSERT(USART_IS_STOPBITS(stopbits));
    ASSERT(USART_IS_PARITY(parity));
    ASSERT(USART_IS_MODE(mode));

    usart_ll_disable(usart->dev);

    usart_ll_set_data_bits(usart->dev, wordlength);
    usart_ll_set_stop_bits(usart->dev, stopbits);

    switch(parity)
    {
        case USART_PARITY_NONE:
            usart_ll_set_parity_mode(usart->dev, USART_LL_PARITY_NONE);
            break;
        case USART_PARITY_EVEN:
            usart_ll_set_parity_mode(usart->dev, USART_LL_PARITY_EVEN);
            break;
        case USART_PARITY_ODD:
            usart_ll_set_parity_mode(usart->dev, USART_LL_PARITY_ODD);
            break;
    }

    usart_ll_set_oversampling_mode(usart->dev, USART_LL_OVERSAMPLING_BY_16);
    usart_ll_set_sample_method(usart->dev, USART_LL_SAMPLE_3);

    ret = usart_hal_set_brr(usart, 16000000U, baudrate);

    if(mode & MODE_TX)
    {
        usart_ll_enable_transmitter(usart->dev);
    }
    else
    {
        usart_ll_disable_transmitter(usart->dev);
    }

    if(mode & MODE_RX)
    {
        usart_ll_enable_receiver(usart->dev);
    }
    else
    {
        usart_ll_disable_receiver(usart->dev);
    }

    usart_ll_enable(usart->dev);

    return ret;
}

error_t usart_hal_tx_dma_setup(usart_hal_context_t* usart, uint8_t dma_circular)
{
    dma_init_t* dma_config;
    uint8_t dma_instance;
    uint8_t dma_stream;
    uint8_t dma_channel;

    ASSERT(usart);

    if(usart->dev == _USART1)
    {
        dma_instance = DMA2;
        dma_stream = DMA_STREAM_7;
        dma_channel = DMA_CHANNEL_4;
    }
    else
    {
        return FAILED;
    }

    usart->tx_dma = dma_hal_init(dma_instance, dma_stream);

    usart->tx_dma->parent = (void*) usart;
    usart->tx_dma->xfer_complete_callback = usart_hal_tx_dma_complete;
    usart->tx_dma->error_callback = usart_hal_tx_dma_error;

    dma_config = &usart->tx_dma->dma_config;
    dma_config->channel = dma_channel;
    dma_config->dbm_enable = 0;
    dma_config->dir = DMA_MEM_TO_PERIPH;
    dma_config->mem_increment = DMA_MEM_INC_ENABLE;
    dma_config->periph_increment = DMA_PERIPH_INC_DISABLE;
    dma_config->periph_data_size = DMA_MEM_SIZE_BYTE;
    dma_config->priority = DMA_PRI_VERY_HIGH;
    if(dma_circular)
    {
        dma_config->mode = DMA_MODE_DIRECT_CIRC;
    }
    else
    {
        dma_config->mode = DMA_MODE_DIRECT;
    }
    

    dma_hal_stream_init(usart->tx_dma);

    return OK;
}

error_t usart_hal_rx_dma_setup(usart_hal_context_t* usart, uint8_t dma_circular)
{
    dma_init_t* dma_config;
    uint8_t dma_instance;
    uint8_t dma_stream;
    uint8_t dma_channel;

    ASSERT(usart);

    if(usart->dev == _USART1)
    {
        dma_instance = DMA2;
        dma_stream = DMA_STREAM_2;
        dma_channel = DMA_CHANNEL_4;
    }
    else
    {
        return FAILED;
    }

    usart->rx_dma = dma_hal_init(dma_instance, dma_stream);
    usart->rx_dma->parent = (void*) usart;
    usart->rx_dma->xfer_complete_callback = usart_hal_rx_dma_complete;
    usart->rx_dma->error_callback = usart_hal_rx_dma_error;
    if(usart->rx_half_complete_callback)
    {
        usart->rx_dma->xfer_half_callback = usart_hal_rx_dma_half_complete;
    }

    dma_config = &usart->rx_dma->dma_config;
    dma_config->channel = dma_channel;
    dma_config->dbm_enable = 0;
    dma_config->dir = DMA_PERIPH_TO_MEM;
    dma_config->mem_increment = DMA_MEM_INC_ENABLE;
    dma_config->periph_increment = DMA_PERIPH_INC_DISABLE;
    dma_config->periph_data_size = DMA_PERIPH_SIZE_BYTE;
    dma_config->priority = DMA_PRI_VERY_HIGH;
    if(dma_circular)
    {
        dma_config->mode = DMA_MODE_DIRECT_CIRC;
    }
    else
    {
        dma_config->mode = DMA_MODE_DIRECT;
    }

    dma_hal_stream_init(usart->rx_dma);

    return OK;
}

error_t usart_hal_enable_tx(usart_hal_context_t* usart)
{
    ASSERT(usart);

    usart_ll_enable_transmitter(usart->dev);

    return OK;
}

error_t usart_hal_disable_tx(usart_hal_context_t* usart)
{
    ASSERT(usart);

    usart_ll_disable_transmitter(usart->dev);

    return OK;
}

error_t usart_hal_enable_rx(usart_hal_context_t* usart)
{
    ASSERT(usart);

    usart_ll_enable_receiver(usart->dev);

    return OK;
}

error_t usart_hal_disable_rx(usart_hal_context_t* usart)
{
    ASSERT(usart);

    usart_ll_disable_receiver(usart->dev);

    return OK;
}

error_t usart_hal_register_callback(usart_hal_context_t* usart, usart_callback_id_t id,
                                                void (*p_callback)(usart_hal_context_t*))
{
    ASSERT(usart);
    ASSERT(USART_IS_CALLBACK(id));
    ASSERT(p_callback);

    switch(id)
    {
        case USART_TX_COMPLETE_CALLBACK:
            usart->tx_complete_callback = p_callback;
            break;
        case USART_RX_COMPLETE_CALLBACK:
            usart->rx_complete_callback = p_callback;
            break;
        case USART_RX_HALF_COMPLETE_CALLBACK:
            usart->rx_half_complete_callback = p_callback;
            break;
        case USART_RX_IDLE_RECEIVED_CALLBACK:
            usart->rx_idle_received_callback = p_callback;
            break;
        case USART_ERROR_CALLBACK:
            usart->error_callback = p_callback;
            break;
        default:
            return FAILED;
    }

    return OK;
}

error_t usart_hal_transmit(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t size)
{
    error_t ret = OK;

    ASSERT(usart);
    ASSERT(pdata);
    ASSERT(size != 0);

    usart->error_code = USART_ERROR_NONE;

    if(((usart_word_length_t) usart_ll_get_data_bits(usart->dev) == USART_WORDLENGTH_9B) && 
            ((usart_parity_t) usart_ll_get_parity_mode(usart->dev) == USART_PARITY_NONE))
    {
        ret = usart_hal_transmit_16b(usart, (const uint16_t*) pdata, size);
    }
    else
    {
        ret = usart_hal_transmit_8b(usart, pdata, size);
    }
    
    return ret;
}

error_t usart_hal_transmit_it(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t sz)
{
    ASSERT(usart);
    ASSERT(pdata);
    ASSERT(sz);

    usart->error_code = USART_ERROR_NONE;

    usart->tx_pbuffer = pdata;
    usart->tx_buffersize = sz;
    usart->tx_count = sz;

    usart_ll_enable_transmit_interrupt(usart->dev);

   return OK;
}

error_t usart_hal_transmit_dma(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t sz)
{
    ASSERT(usart);
    ASSERT(usart->tx_dma);
    ASSERT(pdata);
    ASSERT(sz);

    usart->error_code = USART_ERROR_NONE;
    usart->tx_pbuffer = pdata;
    usart->tx_buffersize = sz;
    usart->tx_count = sz;

    dma_hal_set_transfer(usart->tx_dma, (uint32_t) pdata, (uint32_t) &usart->dev->dr, sz);
    dma_hal_start_it(usart->tx_dma);

    REG_CLR_BIT(usart->dev->sr, USART_SR_TC_S);
    usart_ll_enable_tx_dma(usart->dev);

    return OK;
}

error_t usart_hal_receive(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);

    return usart_hal_receive_polling(usart, pbuffer, size, 0);
}

error_t usart_hal_receive_toidle(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);

    return usart_hal_receive_polling(usart, pbuffer, size, 1);
}

error_t usart_hal_receive_it(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);

    return usart_hal_receive_start_isr(usart, pbuffer, size, 0);
}

error_t usart_hal_receive_it_toidle(usart_hal_context_t* usart, 
                                    uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);

    return usart_hal_receive_start_isr(usart, pbuffer, size, 1);
}

error_t usart_hal_receive_dma(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);
    
    return usart_hal_receive_start_dma(usart, pbuffer, size, 0);
}

error_t usart_hal_receive_dma_toidle(usart_hal_context_t* usart, 
                                    uint8_t* pbuffer, uint16_t size)
{
    ASSERT(usart);
    ASSERT(pbuffer);
    ASSERT(size);
    
    return usart_hal_receive_start_dma(usart, pbuffer, size, 1);
}

uint16_t usart_hal_get_error(usart_hal_context_t* usart)
{
    return usart->error_code;
}

uint16_t usart_hal_get_remaining_rx(usart_hal_context_t* usart)
{
    return usart->rx_count;
}

uint16_t usart_hal_get_remaining_tx(usart_hal_context_t* usart)
{
    return usart->tx_count;
}

uint16_t usart_hal_get_transmitted_count(usart_hal_context_t* usart)
{
    return usart->tx_buffersize - usart->tx_count;
}

uint16_t usart_hal_get_received_count(usart_hal_context_t* usart)
{
    return usart->rx_buffersize - usart->rx_count;
}

/**************STATIC FUNCTION DEFINITIONS*****************************/

static error_t usart_hal_transmit_16b(usart_hal_context_t* usart,
                                        const uint16_t* pdata, uint16_t size)
{    
    ASSERT(pdata);

    while(size > 0)
    {
        while(usart_ll_is_tx_empty(usart->dev) == 0)
        {

        }

        usart_ll_transmit(usart->dev, (uint16_t) (*pdata & 0x01FFU));

        pdata++;
        size--;
    }

    while(usart_ll_is_tx_complete(usart->dev) == 0);
    REG_CLR_BIT(usart->dev->sr, USART_SR_TC_S);

    return OK;
}

static error_t usart_hal_transmit_8b(usart_hal_context_t* usart, 
                                        const uint8_t* pdata, uint16_t size)
{
    ASSERT(pdata);

    while(size > 0)
    {
        while(usart_ll_is_tx_empty(usart->dev) == 0)
        {

        }

        usart_ll_transmit(usart->dev, (uint16_t) (*pdata & 0xFFU));

        pdata++;
        size--;
    }

    while(usart_ll_is_tx_complete(usart->dev) == 0);
    REG_CLR_BIT(usart->dev->sr, USART_SR_TC_S);

    return OK;
}

static error_t usart_hal_transmit_from_isr(usart_hal_context_t* usart)
{
    usart_word_length_t word_length;
    usart_parity_t parity;
    const uint16_t* tmp;

    word_length = (usart_word_length_t) usart_ll_get_data_bits(usart->dev);
    parity = (usart_parity_t) usart_ll_get_parity_mode(usart->dev);

    if(word_length == USART_WORDLENGTH_9B && parity == USART_PARITY_NONE)
    {
        tmp = (const uint16_t*) usart->tx_pbuffer;
        usart_ll_transmit(usart->dev, *tmp & 0x01FF);
        usart->tx_count--;
        usart->tx_pbuffer += 2;
    }
    else
    {
        usart_ll_transmit(usart->dev, (uint16_t) (*usart->tx_pbuffer & 0xFF));
        usart->tx_count--;
        usart->tx_pbuffer++;
    }

    if(usart->tx_count == 0)
    {
        usart_ll_enable_tc_interrupt(usart->dev);
        usart_ll_disable_transmit_interrupt(usart->dev);
    }

    return OK;
}

static void usart_hal_tx_dma_complete(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(dma->parent);

    usart_hal_context_t* usart = (usart_hal_context_t*) (dma->parent);
    usart->tx_count = 0;

    if((dma->dma_config.mode & DMA_CIRC) == 0)
    {
        usart_ll_disable_tx_dma(usart->dev);
        usart_ll_enable_tc_interrupt(usart->dev);
    }
    else
    {
        if(usart->tx_complete_callback)
        {
            usart->tx_complete_callback(usart);
        }
    }
}

static void usart_hal_tx_dma_error(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(dma->parent);

    usart_hal_context_t* usart = (usart_hal_context_t*) (dma->parent);
    usart->error_code |= USART_ERROR_DMA;

    if(dma->error_code & DMA_ERROR_TE)
    {
        usart->tx_count = dma->remaining;

        usart_ll_disable_tx_dma(usart->dev);
        usart_ll_disable_tc_interrupt(usart->dev);
    }

    if(usart->error_callback)
    {
        usart->error_callback(usart);
    }
}

static error_t usart_hal_receive_polling(usart_hal_context_t* usart, 
                                        uint8_t* pbuffer, uint16_t size, uint8_t toidle)
{
    usart_word_length_t word_length;
    usart_parity_t parity;
    uint16_t* pbuff_16;
    uint8_t* pbuff_8;

    pbuff_16 = NULL;
    pbuff_8 = NULL;
    usart->error_code = USART_ERROR_NONE;
    usart->rx_buffersize = size;
    usart->rx_count = size;
    
    word_length = (usart_word_length_t) usart_ll_get_data_bits(usart->dev);
    ASSERT(USART_IS_WORDLENGTH(word_length));

    switch(usart_ll_get_parity_mode(usart->dev))
    {
        case USART_LL_PARITY_NONE:
            parity = USART_PARITY_NONE;
            break;
        case USART_LL_PARITY_EVEN:
            parity = USART_PARITY_EVEN;
            break;
        case USART_LL_PARITY_ODD:
            parity = USART_PARITY_ODD;
            break;
        default:
            parity = USART_PARITY_INV;
            break;
    }
    ASSERT(USART_IS_PARITY(parity));

    if(word_length == USART_WORDLENGTH_9B && parity == USART_PARITY_NONE)
    {
        pbuff_16 = (uint16_t*) pbuffer;
    }
    else
    {
        pbuff_8 = (uint8_t*) pbuffer;
    }

    while(usart->rx_count)
    {
        while(!usart_ll_is_rx_full(usart->dev))
        {
            /* TODO: Add Timeout */
            
            /*Handling IDLE flag*/
            if(toidle && usart_ll_idle_detected(usart->dev))
            {
                usart_ll_clear_idle_flag(usart->dev);

                if(usart->rx_count < usart->rx_buffersize)
                {
                    return OK;
                }
            }
        }

        if(usart_ll_is_parity_error(usart->dev) && parity != USART_PARITY_NONE)
        {
            usart_ll_clear_parity_error(usart->dev);
            usart->error_code |= USART_ERROR_PE;
        }

        if(usart_ll_is_noise_error(usart->dev))
        {
            usart_ll_clear_noise_error(usart->dev);
            usart->error_code |= USART_ERROR_NE;
        }

        if(usart_ll_is_framing_error(usart->dev))
        {
            usart_ll_clear_framing_error(usart->dev);
            usart->error_code |= USART_ERROR_FE;
        }

        if(usart_ll_is_overrun_error(usart->dev))
        {
            usart_ll_clear_overrun_error(usart->dev);
            usart->error_code |= USART_ERROR_ORE;
        }

        if((usart->error_code == USART_ERROR_NONE) || (usart->error_code == USART_ERROR_ORE))
        {
            if(pbuff_8)
            {
                if(parity == USART_PARITY_NONE)
                {
                    *pbuff_8 = (uint8_t) usart_ll_receive(usart->dev) & 0xFF;
                }
                else
                {
                    if(word_length == USART_WORDLENGTH_8B)
                    {
                        *pbuff_8 = (uint8_t) usart_ll_receive(usart->dev) & 0x7F;
                    }
                    else
                    {
                        *pbuff_8 = (uint8_t) usart_ll_receive(usart->dev) & 0xFF;
                    }
                }
                
                pbuff_8++;
            }
            else
            {
                *pbuff_16 = usart_ll_receive(usart->dev) & 0x01FF;
                pbuff_16++;
            }

            usart->rx_count--;
        }

        if(usart->error_code != USART_ERROR_NONE)
        {
            return FAILED;
        }
    }

    return OK;
}

static error_t usart_hal_receive_start_isr(usart_hal_context_t* usart, 
                                            uint8_t* pbuffer, uint16_t size, uint8_t toidle)
{
    usart->rx_pbuffer = pbuffer;
    usart->rx_buffersize = size;
    usart->rx_count = size;

    usart->error_code = USART_ERROR_NONE;

    usart_ll_clear_framing_error(usart->dev);
    usart_ll_clear_noise_error(usart->dev);
    usart_ll_clear_overrun_error(usart->dev);
    usart_ll_clear_parity_error(usart->dev);
    usart_ll_clear_receive_flag(usart->dev);
    usart_ll_clear_idle_flag(usart->dev);

    if(toidle)
    {
        usart_ll_enable_idle_interrupt(usart->dev);    
    }

    usart_ll_enable_receive_interrupt(usart->dev);

    return OK;
}

static error_t usart_hal_receive_start_dma(usart_hal_context_t* usart,
                                        uint8_t* pbuffer, uint16_t size, uint8_t toidle)
{
    usart_parity_t parity;

    usart->rx_pbuffer = pbuffer;
    usart->rx_buffersize = size;
    usart->rx_count = size;

    switch(usart_ll_get_parity_mode(usart->dev))
    {
        case USART_LL_PARITY_NONE:
            parity = USART_PARITY_NONE;
            break;
        case USART_LL_PARITY_EVEN:
            parity = USART_PARITY_EVEN;
            break;
        case USART_LL_PARITY_ODD:
            parity = USART_PARITY_ODD;
            break;
        default:
            parity = USART_PARITY_INV;
            break;
    }
    ASSERT(USART_IS_PARITY(parity));

    dma_hal_set_transfer(usart->rx_dma, (uint32_t) &usart->dev->dr, (uint32_t) pbuffer, size);
    dma_hal_start_it(usart->rx_dma);

    usart_ll_clear_framing_error(usart->dev);
    usart_ll_clear_noise_error(usart->dev);
    usart_ll_clear_overrun_error(usart->dev);
    usart_ll_clear_parity_error(usart->dev);
    usart_ll_clear_idle_flag(usart->dev);
    usart_ll_clear_receive_flag(usart->dev);

    usart_ll_enable_error_intterrupt(usart->dev);
    if(parity != USART_PARITY_NONE)
    {
        usart_ll_enable_parity_interrupt(usart->dev);
    }
    if(toidle)
    {
        usart_ll_enable_idle_interrupt(usart->dev);
    }

    usart_ll_enable_rx_dma(usart->dev);

    return OK;
}

static error_t usart_hal_receive_from_isr(usart_hal_context_t* usart)
{
    uint16_t* pbuff_16;
    uint8_t* pbuff_8;
    usart_parity_t parity;
    usart_word_length_t wordlength;

    ASSERT(usart);
    
    pbuff_16 = NULL;
    pbuff_8 = NULL;

    wordlength = usart_ll_get_data_bits(usart->dev);
    
    switch(usart_ll_get_parity_mode(usart->dev))
    {
        case USART_LL_PARITY_NONE:
            parity = USART_PARITY_NONE;
            break;
        case USART_LL_PARITY_EVEN:
            parity = USART_PARITY_EVEN;
            break;
        case USART_LL_PARITY_ODD:
            parity = USART_PARITY_ODD;
            break;
        default:
            parity = USART_PARITY_INV;
            break;
    }
    ASSERT(USART_IS_PARITY(parity));

    if(usart_ll_is_parity_error(usart->dev) && parity != USART_PARITY_NONE)
    {
        usart_ll_clear_parity_error(usart->dev);
        usart->error_code |= USART_ERROR_PE;
    }
    if(usart_ll_is_noise_error(usart->dev))
    {
        usart_ll_clear_noise_error(usart->dev);
        usart->error_code |= USART_ERROR_NE;
    }
    if(usart_ll_is_framing_error(usart->dev))
    {
        usart_ll_clear_framing_error(usart->dev);
        usart->error_code |= USART_ERROR_FE;
    }
    if(usart_ll_is_overrun_error(usart->dev))
    {
        usart_ll_clear_overrun_error(usart->dev);
        usart->error_code |= USART_ERROR_ORE;
    }

    if((usart->error_code == USART_ERROR_NONE) || (usart->error_code == USART_ERROR_ORE))
    {
        if(wordlength == USART_WORDLENGTH_9B && parity == USART_PARITY_NONE)
        {
            pbuff_16 = (uint16_t*) usart->rx_pbuffer;
            *pbuff_16 = usart_ll_receive(usart->dev) & 0x01FF;
            usart->rx_pbuffer += 2;
        }
        else if(wordlength == USART_WORDLENGTH_9B || parity == USART_PARITY_NONE)
        {
            pbuff_8 = usart->rx_pbuffer;
            *pbuff_8 = (uint8_t) usart_ll_receive(usart->dev) & 0xFF;
            usart->rx_pbuffer++;
        }
        else
        {
            pbuff_8 = usart->rx_pbuffer;
            *pbuff_8 = (uint8_t) usart_ll_receive(usart->dev) & 0x7F;
            usart->rx_pbuffer++;
        }

        usart->rx_count--;
    }

    if(usart->error_code != USART_ERROR_NONE)
    {
        usart_ll_disable_receive_interrupt(usart->dev);
        if(usart->error_callback)
        {
            usart->error_callback(usart);
        }

        return FAILED;
    }

    if(usart->rx_count == 0)
    {
        usart_ll_disable_receive_interrupt(usart->dev);
        if(usart->rx_complete_callback)
        {
            usart->rx_complete_callback(usart);
        }
    }

    return OK;
}

static void usart_hal_rx_dma_complete(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(dma->parent);

    usart_hal_context_t* usart = (usart_hal_context_t*) dma->parent;
    usart->rx_count = 0;

    if((dma->dma_config.mode & DMA_CIRC) == 0)
    {
        usart_ll_disable_error_intterrupt(usart->dev);
        usart_ll_disable_parity_interrupt(usart->dev);
        usart_ll_disable_rx_dma(usart->dev);
    }

    if(usart->rx_complete_callback)
    {
        usart->rx_complete_callback(usart);
    }    
}

static error_t usart_hal_multibuffer_error(usart_hal_context_t* usart)
{
    if(usart_ll_parity_interrupt_enabled(usart->dev) && 
                usart_ll_parity_interrupt_enabled(usart->dev))
    {
        usart_ll_clear_parity_error(usart->dev);
        usart->error_code |= USART_ERROR_PE;
    }
    if(usart_ll_is_noise_error(usart->dev) && 
                usart_ll_error_intterrupt_enabled(usart->dev))
    {
        usart_ll_clear_noise_error(usart->dev);
        usart->error_code |= USART_ERROR_NE;
    }
    if(usart_ll_is_framing_error(usart->dev) &&
                usart_ll_error_intterrupt_enabled(usart->dev))
    {
        usart_ll_clear_framing_error(usart->dev);
        usart->error_code |= USART_ERROR_FE;
    }
    if(usart_ll_is_overrun_error(usart->dev) &&
                usart_ll_error_intterrupt_enabled(usart->dev))
    {
        usart_ll_clear_overrun_error(usart->dev);
        usart->error_code |= USART_ERROR_ORE;
    }

    if(usart->error_code != USART_ERROR_NONE)
    {
        usart_ll_disable_parity_interrupt(usart->dev);
        usart_ll_disable_error_intterrupt(usart->dev);
        usart_ll_disable_rx_dma(usart->dev);
        dma_hal_abort_from_isr(usart->rx_dma);

        usart->rx_count = usart->rx_dma->remaining;

        if(usart->error_callback)
        {
            usart->error_callback(usart);
        }
    }

    return OK;
}

static void usart_hal_rx_dma_error(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(dma->parent);

    usart_hal_context_t* usart = (usart_hal_context_t*) dma->parent;
    usart->error_code |= USART_ERROR_DMA;

    if(dma->error_code & DMA_ERROR_TE)
    {
        usart->rx_count = dma->remaining;
        usart_ll_disable_parity_interrupt(usart->dev);
        usart_ll_disable_error_intterrupt(usart->dev);
        usart_ll_disable_rx_dma(usart->dev);
        if(usart->error_callback)
        {
            usart->error_callback(usart);
        }
    }
}

static void usart_hal_rx_dma_half_complete(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(dma->parent);

    usart_hal_context_t* usart = (usart_hal_context_t*) dma->parent;

    if(usart->rx_half_complete_callback)
    {
        usart->rx_half_complete_callback(usart);
    }
}

static error_t usart_hal_set_brr(usart_hal_context_t* usart, uint32_t pclk, uint32_t baudrate)
{
    float brr_val;
    uint32_t sample_rate;
    uint16_t brr_mantissa;
    uint8_t brr_fraction;

    ASSERT(usart);
    
    if(usart_ll_get_oversamling_mode(usart->dev) == USART_LL_OVERSAMPLING_BY_16)
    {
        sample_rate = baudrate << 4;
        brr_val = (float) pclk / sample_rate;

        /* Get the integer part in brr_mantissa */
        brr_mantissa = (uint16_t) brr_val;

        /* Get the fractional part in brr_val */
        brr_val = brr_val - (float) brr_mantissa;
        brr_val *= 16.0f;   /* From 0.0 to 15.99 */
        brr_fraction = (uint8_t) (brr_val + 0.5f); /* Round to nearest int */

        brr_mantissa += (brr_fraction & 0x10 ? 1 : 0);
        brr_fraction &= 0x0F;
    }
    else
    {
        sample_rate = baudrate << 3;
        brr_val = pclk / sample_rate;

        /* Get the integer part in brr_mantissa */
        brr_mantissa = (uint16_t) brr_val;

        /* Get the fractional part in brr_val */
        brr_val = brr_val - (float) brr_mantissa;
        brr_val *= 8.0f; /* From 0.0 to 7.99 */
        brr_fraction = (uint8_t) (brr_val + 0.5f);

        brr_mantissa += (brr_fraction & 0x08 ? 1 : 0);
        brr_fraction &= 0x07;
    }

    usart_ll_brr_set_mantissa(usart->dev, brr_mantissa);
    usart_ll_brr_set_fraction(usart->dev, brr_fraction);

    return OK;
}

WEAK void usart_hal_irq_handler(usart_hal_context_t* usart)
{
    uint32_t error_flags =  usart_ll_is_parity_error(usart->dev)    || 
                            usart_ll_is_framing_error(usart->dev)   ||
                            usart_ll_is_noise_error(usart->dev)     ||
                            usart_ll_is_overrun_error(usart->dev);
    uint16_t nb_remaining;
    /* USART PE/ERROR interrupt */
    if((error_flags != USART_ERROR_NONE) && usart_ll_is_rx_dma_enabled(usart->dev))
    {
        usart_hal_multibuffer_error(usart);
    }

    /* USART IDLE interrupt */
    if(usart_ll_idle_detected(usart->dev) && usart_ll_idle_interrupt_enabled(usart->dev))
    {
        usart_ll_clear_idle_flag(usart->dev);

        if(usart_ll_is_rx_dma_enabled(usart->dev))
        {
            nb_remaining = dma_hal_get_remaining_items(usart->rx_dma);
            if((nb_remaining > 0) && (nb_remaining < usart->rx_buffersize))
            {
                if((usart->rx_dma->dma_config.mode & DMA_CIRC) == 0)
                {
                    usart_ll_disable_parity_interrupt(usart->dev);
                    usart_ll_disable_error_intterrupt(usart->dev);
                    usart_ll_disable_idle_interrupt(usart->dev);
                    usart_ll_disable_rx_dma(usart->dev);
                    dma_hal_abort_from_isr(usart->rx_dma);
                }
                
                usart->rx_count = nb_remaining;

                if(usart->rx_idle_received_callback)
                {
                    usart->rx_idle_received_callback(usart);
                }
            }
        }
        else
        {
            if(usart->rx_count < usart->rx_buffersize)
            {
                usart_ll_disable_idle_interrupt(usart->dev);
                usart_ll_disable_receive_interrupt(usart->dev);

                if(usart->rx_idle_received_callback)
                {
                    usart->rx_idle_received_callback(usart);
                }
            }
        }
    }

    /* USART RXNE interrupt */
    if((usart_ll_is_rx_full(usart->dev) && usart_ll_receive_interrupt_enabled(usart->dev)))
    {
        usart_hal_receive_from_isr(usart);
        return;
    }

    /* USART TXE interrupt */
    if(usart_ll_is_tx_empty(usart->dev) && 
            usart_ll_transmit_interrupt_enabled(usart->dev))
    {
        usart_hal_transmit_from_isr(usart);
    }

    /* USART TC interrupt */
    if(usart_ll_is_tx_complete(usart->dev) && 
            usart_ll_tc_interrupt_enabled(usart->dev))
    {
        usart_ll_disable_tc_interrupt(usart->dev);
        REG_CLR_BIT(usart->dev->sr, USART_SR_TC_S);
        
        if(usart->tx_complete_callback)
        {
            usart->tx_complete_callback(usart);
        }
    }
}

void usart1_irq_handler(void)
{
    usart_hal_irq_handler(&usartx[USART1]);
}

void usart2_irq_handler(void)
{

}

void usart3_irq_handler(void)
{

}
