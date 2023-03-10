#include "dma_hal_ext.h"

#include "stm32f446xx.h"
#include "rcc_hal_ext.h"
#include "assert.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define DMA_GET_HDMA(id, stream)    (hdma + ((id) * DMA_LL_DEVS + (stream)))

#define DMA_IS_INSTANCE(id)         ((id) < DMA_INV)
#define DMA_IS_STREAM(_st)          ((_st) < DMA_STREAM_MAX)
#define DAM_IS_CHANNEL(_ch)         ((_ch) < DMA_CHANNEL_MAX)
#define DMA_IS_PRIORITY(_pri)       ((_pri) < DMA_PRI_INV)
#define DMA_IS_MEM_SIZE(_sz)        ((_sz) < DMA_MEM_SIZE_MAX)
#define DMA_IS_PERIPH_SIZE(_sz)     ((_sz) < DMA_PERIPH_SIZE_MAX)
#define DMA_IS_DIRECTION(_dir)      ((_dir) <= DMA_MEM_TO_MEM)
#define DMA_IS_MODE(_mode)          ((_mode) == DMA_MODE_DIRECT || \
                                                        (_mode) == DMA_MODE_DIRECT_CIRC)

#define DMA_IS_WORD_ALIGNED(_addr)  (((_addr) & 0x03) == 0)
#define DMA_IS_HWORD_ALIGNED(_addr) (((_addr) & 0x01) == 0)

/* PRIVATE VARIABLES */

static dma_hal_context_t hdma[DMA_LL_DEVS * DMA_STREAM_MAX];

/* PRIVATE FUNCTIONS DECLARATION */

static error_t dma_hal_check_config(dma_hal_context_t* dma);

/* PUBLIC FUNCTIONS DEFINITION */

dma_hal_context_t* dma_hal_init(uint8_t dma_instance, uint8_t stream)
{
    rcc_hal_context_t rcc;
    dma_hal_context_t* dma;

    ASSERT(DMA_IS_INSTANCE(dma_instance));
    ASSERT(DMA_IS_STREAM(stream));

    RCC_HAL_GET_HW(&rcc, RCC);

    switch(dma_instance)
    {
        case DMA1:
            rcc_hal_ahb1_en_clk(&rcc, RCC_HAL_DMA1);

            switch(stream)
            {
                case DMA_STREAM_0:
                    NVIC_EnableIRQ(DMA1_Stream0_IRQn);    
                    break;
                case DMA_STREAM_1:
                    NVIC_EnableIRQ(DMA1_Stream1_IRQn);    
                    break;
                case DMA_STREAM_2:
                    NVIC_EnableIRQ(DMA1_Stream2_IRQn);    
                    break;
                case DMA_STREAM_3:
                    NVIC_EnableIRQ(DMA1_Stream3_IRQn);    
                    break;
                case DMA_STREAM_4:
                    NVIC_EnableIRQ(DMA1_Stream4_IRQn);    
                    break;
                case DMA_STREAM_5:
                    NVIC_EnableIRQ(DMA1_Stream5_IRQn);    
                    break;
                case DMA_STREAM_6:
                    NVIC_EnableIRQ(DMA1_Stream6_IRQn);    
                    break;
                case DMA_STREAM_7:
                    NVIC_EnableIRQ(DMA1_Stream7_IRQn);    
                    break;
            }
            break;
        case DMA2:
            rcc_hal_ahb1_en_clk(&rcc, RCC_HAL_DMA2);

            switch(stream)
            {
                case DMA_STREAM_0:
                    NVIC_EnableIRQ(DMA2_Stream0_IRQn);    
                    break;
                case DMA_STREAM_1:
                    NVIC_EnableIRQ(DMA2_Stream1_IRQn);    
                    break;
                case DMA_STREAM_2:
                    NVIC_EnableIRQ(DMA2_Stream2_IRQn);    
                    break;
                case DMA_STREAM_3:
                    NVIC_EnableIRQ(DMA2_Stream3_IRQn);    
                    break;
                case DMA_STREAM_4:
                    NVIC_EnableIRQ(DMA2_Stream4_IRQn);    
                    break;
                case DMA_STREAM_5:
                    NVIC_EnableIRQ(DMA2_Stream5_IRQn);    
                    break;
                case DMA_STREAM_6:
                    NVIC_EnableIRQ(DMA2_Stream6_IRQn);    
                    break;
                case DMA_STREAM_7:
                    NVIC_EnableIRQ(DMA2_Stream7_IRQn);    
                    break;
            }
            break;        
    }

    dma = DMA_GET_HDMA(dma_instance, stream);
    memset((void*) dma, 0, sizeof(dma_hal_context_t));
    DMA_HAL_GET_HW(dma, dma_instance);
    ASSERT(dma->dev);
    dma->stream = stream;

    return dma;
}

error_t dma_hal_stream_init(dma_hal_context_t* dma)
{
    dma_init_t* dma_config;

    ASSERT(dma);
    dma_config = &dma->dma_config;

    ASSERT(DMA_IS_STREAM(dma->stream));
    ASSERT(DAM_IS_CHANNEL(dma_config->channel));
    ASSERT(DMA_IS_PRIORITY(dma_config->priority));
    ASSERT(DMA_IS_MEM_SIZE(dma_config->mem_data_size));
    ASSERT(DMA_IS_PERIPH_SIZE(dma_config->periph_data_size));
    ASSERT(DMA_IS_DIRECTION(dma_config->dir));
    ASSERT(DMA_IS_MODE(dma_config->mode));
    ASSERT(dma_hal_check_config(dma) == OK);

    dma_ll_disable_stream(dma->dev, dma->stream);

    while(dma_ll_is_stream_enabled(dma->dev, dma->stream))
    {

    }

    dma_ll_set_channel(dma->dev, dma->stream, dma_config->channel);
    dma_ll_set_priority(dma->dev, dma->stream, dma_config->priority);
    dma_ll_set_periph_transfer_size(dma->dev, dma->stream, dma_config->periph_data_size);

    if(dma_config->mode & DMA_FIFO)
    {
        dma_ll_set_mem_transfer_size(dma->dev, dma->stream, dma_config->mem_data_size);
    }
    
    if(dma_config->mem_increment)
    {
        dma_ll_enable_mem_inc(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_disable_mem_inc(dma->dev, dma->stream);
    }

    if(dma_config->periph_increment)
    {
        dma_ll_enable_periph_inc(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_disable_periph_inc(dma->dev, dma->stream);
    }

    if(dma_config->mode & DMA_NORMAL)
    {
        dma_ll_enable_direct_mode(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_disable_direct_mode(dma->dev, dma->stream);
    }
    
    if(dma_config->mode & DMA_CIRC)
    {
        dma_ll_enable_circular_mode(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_disable_circular_mode(dma->dev, dma->stream);
    }

    if(dma_config->dbm_enable)
    {
        dma_ll_enable_double_buffer(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_disable_double_buffer(dma->dev, dma->stream);
    }

    dma_ll_set_direction(dma->dev, dma->stream, dma_config->dir);

    dma_ll_clear_flags(dma->dev, dma->stream);

    dma->error_code = DMA_ERROR_NONE;

    return OK;
}

error_t dma_hal_set_transfer(dma_hal_context_t* dma, 
            uint32_t src_addr, uint32_t dest_addr, uint16_t len)
{
    dma_init_t* dma_config;

    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    dma_config = &dma->dma_config;

    while(dma_ll_is_stream_enabled(dma->dev, dma->stream))
    {

    }

    /*
    * TODO: Disable DBM mode
    */

    if(dma_config->dir == DMA_PERIPH_TO_MEM)
    {
        if(dma_config->periph_data_size == DMA_PERIPH_SIZE_WORD)
        {
            ASSERT(DMA_IS_WORD_ALIGNED(src_addr));
        }
        else if(dma_config->periph_data_size == DMA_PERIPH_SIZE_HALF_WORD)
        {
            ASSERT(DMA_IS_HWORD_ALIGNED(src_addr));
        }

        if(dma_config->mem_data_size == DMA_MEM_SIZE_WORD)
        {
            ASSERT(DMA_IS_WORD_ALIGNED(dest_addr));
        }
        else if(dma_config->mem_data_size == DMA_MEM_SIZE_HALF_WORD)
        {
            ASSERT(DMA_IS_HWORD_ALIGNED(dest_addr));
        }

        dma_ll_set_periph_addr(dma->dev, dma->stream, src_addr);
        dma_ll_set_mem_addr_0(dma->dev, dma->stream, dest_addr);
    }
    else if(dma_config->dir == DMA_MEM_TO_PERIPH)
    {
        if(dma_config->periph_data_size == DMA_PERIPH_SIZE_WORD)
        {
            ASSERT(DMA_IS_WORD_ALIGNED(dest_addr));
        }
        else if(dma_config->periph_data_size == DMA_PERIPH_SIZE_HALF_WORD)
        {
            ASSERT(DMA_IS_HWORD_ALIGNED(dest_addr));
        }

        if(dma_config->mem_data_size == DMA_MEM_SIZE_WORD)
        {
            ASSERT(DMA_IS_WORD_ALIGNED(src_addr));
        }
        else if(dma_config->mem_data_size == DMA_MEM_SIZE_HALF_WORD)
        {
            ASSERT(DMA_IS_HWORD_ALIGNED(src_addr));
        }

        dma_ll_set_periph_addr(dma->dev, dma->stream, dest_addr);
        dma_ll_set_mem_addr_0(dma->dev, dma->stream, src_addr);
    }
    else
    {
        return FAILED;
    }

    dma_ll_set_number_of_transfers(dma->dev, dma->stream, len);

    return OK;
}

error_t dma_hal_start(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    dma->error_code = DMA_ERROR_NONE;

    dma_ll_clear_flags(dma->dev, dma->stream);
    dma_ll_enable_stream(dma->dev, dma->stream);

    return OK;
}

error_t dma_hal_start_it(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    dma->error_code = DMA_ERROR_NONE;

    dma_ll_clear_flags(dma->dev, dma->stream);
    dma_ll_enable_transfer_error_it(dma->dev, dma->stream);
    dma_ll_enable_direct_mode_error_it(dma->dev, dma->stream);
    dma_ll_enable_fifo_error_it(dma->dev, dma->stream);
    dma_ll_enable_complete_transfer_it(dma->dev, dma->stream);

    if(dma->xfer_half_callback)
    {
        dma_ll_enable_half_transfer_it(dma->dev, dma->stream);
    }

    dma_ll_enable_stream(dma->dev, dma->stream);

    return OK;
}

error_t dma_hal_poll_for_transfer(dma_hal_context_t* dma, uint8_t level)
{
    uint32_t error;
    uint8_t completed;

    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    /*
    * TODO: Handle CIRC mode
    */
   
    error = DMA_ERROR_NONE;
    completed = 0;

    while( !completed && (error & DMA_ERROR_TE) == 0)
    {
        if(dma_ll_get_transfer_error_flag(dma->dev, dma->stream))
        {
            error |= DMA_ERROR_TE;
            dma_ll_clear_transfer_error_flag(dma->dev, dma->stream);
        }

        if(dma_ll_get_direct_mode_error_flag(dma->dev, dma->stream))
        {
            error |= DMA_ERROR_DME;
            dma_ll_clear_direct_mode_error_flag(dma->dev, dma->stream);
        }

        if(dma_ll_get_fifo_error_flag(dma->dev, dma->stream))
        {
            error |= DMA_ERROR_FE;
            dma_ll_clear_fifo_error_flag(dma->dev, dma->stream);
        }

        if((level == DMA_LEVEL_COMPLETE) && 
                    dma_ll_get_transfer_complete_flag(dma->dev, dma->stream))
        {
            completed = 1;
        }
        else if((level == DMA_LEVEL_HALF) && 
                    dma_ll_get_half_transfer_flag(dma->dev, dma->stream))
        {
            completed = 1;
        }
    }

    if(error & DMA_ERROR_TE)
    {
        dma_hal_abort(dma);
        dma->error_code = error;

        return FAILED;
    }

    if(level == DMA_LEVEL_COMPLETE)
    {
        dma_ll_clear_transfer_complete_flag(dma->dev, dma->stream);
        dma_ll_clear_half_transfer_flag(dma->dev, dma->stream);
    }
    else
    {
        dma_ll_clear_half_transfer_flag(dma->dev, dma->stream);
    }

    dma->error_code = error;

    return OK;
}

error_t dma_hal_abort(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    dma_ll_disable_stream(dma->dev, dma->stream);
    while(dma_ll_is_stream_enabled(dma->dev, dma->stream))
    {

    }

    dma->remaining = dma_ll_get_remaining_items(dma->dev, dma->stream);

    dma_ll_clear_flags(dma->dev, dma->stream);

    return OK;
}

error_t dma_hal_abort_from_isr(dma_hal_context_t* dma)
{
    ASSERT(dma);
    ASSERT(DMA_IS_STREAM(dma->stream));

    dma_ll_disable_stream(dma->dev, dma->stream);

    dma->remaining = dma_ll_get_remaining_items(dma->dev, dma->stream);

    dma_ll_clear_flags(dma->dev, dma->stream);

    return OK;
}

uint16_t dma_hal_get_remaining_items(dma_hal_context_t* dma)
{
    ASSERT(dma);

    return dma_ll_get_remaining_items(dma->dev, dma->stream);
}

static error_t dma_hal_check_config(dma_hal_context_t* dma)
{
    ASSERT(dma);

    error_t ret = OK;
    dma_init_t* dma_config = &dma->dma_config;

    if(dma_config->dir == DMA_MEM_TO_MEM)
    {
        if(((dma_config->mode & DMA_FIFO) == 0) || dma->dev != _DMA2)
        {
            ret = FAILED;
        }

        if(dma_config->mode & DMA_CIRC)
        {
            ret = FAILED;
        }
    }

    if(dma_config->mem_data_size != dma_config->periph_data_size)
    {
        if((dma_config->mode & DMA_FIFO) == 0)
        {
            ret = FAILED;
        }
    }

    return ret;
}

WEAK void dma_irq_handler(dma_hal_context_t* dma)
{
    /* TEI*/
    if(dma_ll_get_transfer_error_flag(dma->dev, dma->stream))
    {
        if(dma_ll_is_transfer_error_it_enabled(dma->dev, dma->stream))
        {
            dma_ll_disable_transfer_error_it(dma->dev, dma->stream);
            dma_ll_clear_transfer_error_flag(dma->dev, dma->stream);
            dma->error_code |= DMA_ERROR_TE;
        }
    }

    if(dma_ll_get_direct_mode_error_flag(dma->dev, dma->stream))
    {
        if(dma_ll_is_direct_mode_error_it_enabled(dma->dev, dma->stream))
        {
            dma_ll_clear_direct_mode_error_flag(dma->dev, dma->stream);
            dma->error_code |= DMA_ERROR_DME;
        }
    }

    if(dma_ll_get_fifo_error_flag(dma->dev, dma->stream))
    {
        if(dma_ll_is_fifo_error_it_enabled(dma->dev, dma->stream))
        {
            dma_ll_clear_fifo_error_flag(dma->dev, dma->stream);
            dma->error_code |= DMA_ERROR_FE;
        }
    }

    if(dma_ll_get_half_transfer_flag(dma->dev, dma->stream))
    {
        if(dma_ll_is_half_transfer_it_enabled(dma->dev, dma->stream))
        {
            if((dma->dma_config.mode & DMA_CIRC) == 0)
            {
                dma_ll_disable_half_transfer_it(dma->dev, dma->stream);
            }

            dma_ll_clear_half_transfer_flag(dma->dev, dma->stream);

            if(dma->xfer_half_callback)
            {
                dma->xfer_half_callback(dma);
            }
        }
    }

    if(dma_ll_get_transfer_complete_flag(dma->dev, dma->stream))
    {
        if(dma_ll_is_transfer_complete_it_enabled(dma->dev, dma->stream))
        {
            if((dma->dma_config.mode & DMA_CIRC) == 0)
            {
                dma_ll_disable_complete_transfer_it(dma->dev, dma->stream);
                dma_ll_disable_transfer_error_it(dma->dev, dma->stream);
                dma_ll_disable_direct_mode_error_it(dma->dev, dma->stream);
                dma_ll_disable_fifo_error_it(dma->dev, dma->stream);
                dma_ll_disable_half_transfer_it(dma->dev, dma->stream);
            }

            dma_ll_clear_transfer_complete_flag(dma->dev, dma->stream);

            if(dma->xfer_complete_callback)
            {
                dma->xfer_complete_callback(dma);
            }
        }
    }

    if(dma->error_code != DMA_ERROR_NONE)
    {
        if(dma->error_code & DMA_ERROR_TE)
        {
            dma_hal_abort_from_isr(dma);
        }

        if(dma->error_callback)
        {
            dma->error_callback(dma);
        }
    }
}

void dma1_stream0_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_0));
}

void dma1_stream1_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_1));
}

void dma1_stream2_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_2));
}

void dma1_stream3_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_3));
}

void dma1_stream4_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_4));
}

void dma1_stream5_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_5));
}

void dma1_stream6_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_6));
}

void dma1_stream7_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA1, DMA_STREAM_7));
}

/* DMA2 IRQs */
void dma2_stream0_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_0));
}

void dma2_stream1_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_1));
}

void dma2_stream2_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_2));
}

void dma2_stream3_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_3));
}

void dma2_stream4_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_4));
}

void dma2_stream5_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_5));
}

void dma2_stream6_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_6));
}

void dma2_stream7_irq_handler(void)
{
    dma_irq_handler(DMA_GET_HDMA(DMA2, DMA_STREAM_7));
}

