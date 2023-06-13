#include "serial.h"

#include "assert.h"

#include <string.h>

#define SERIAL_IS_PORT(_port)   ((_port) < SERIAL_PORT_MAX)

static serial_t serial_ports[SERIAL_PORT_MAX];

/* 
/------------------Private function declarations ------------------------/
*/

static void serial_tx_complete_handler(usart_hal_context_t* p_usart);
static void serial_rx_complete_handler(usart_hal_context_t* p_usart);
static void serial_rx_hlf_complete_handler(usart_hal_context_t* p_usart);
static void serial_rx_idle_handler(usart_hal_context_t* p_usart);


static error_t serial_init(uint8_t port)
{
    usart_hal_context_t* p_usart;
    error_t ret = FAILED;

    ASSERT(SERIAL_IS_PORT(port));

    p_usart = usart_hal_init(port);
    if(p_usart)
    {
        usart_hal_register_callback(p_usart, USART_TX_COMPLETE_CALLBACK, 
                                                    serial_tx_complete_handler);
        usart_hal_register_callback(p_usart, USART_RX_COMPLETE_CALLBACK,
                                                        serial_rx_complete_handler);
        usart_hal_register_callback(p_usart, USART_RX_HALF_COMPLETE_CALLBACK,
                                                    serial_rx_hlf_complete_handler);
        usart_hal_register_callback(p_usart, USART_RX_IDLE_RECEIVED_CALLBACK,
                                                            serial_rx_idle_handler);    

        usart_hal_tx_dma_setup(p_usart, 0);
        usart_hal_rx_dma_setup(p_usart, 1);
        
        memset((void*) &serial_ports[port], 0, sizeof(serial_t));
        serial_ports[port].usart = p_usart;
        serial_ports[port].tx_queue_size = sizeof(serial_ports[port].tx_queue);
        serial_ports[port].rx_queue_size = sizeof(serial_ports[port].rx_queue);
        serial_ports[port].tx_status = SERIAL_TX_STATUS_IDLE;

        ret = OK;
    }

    return ret;
}

error_t serial_setup(uint8_t port, const serial_config_t* config)
{
    serial_t* p_port;
    uint8_t word_length;
    uint8_t stop_bits;
    error_t ret;

    ASSERT(SERIAL_IS_PORT(port));
    ASSERT(config != NULL);
    ASSERT(!((config->parity == SERIAL_PARITY_NONE) && 
            (config->data_bits == SERIAL_DATA_BITS_7)));
    
    p_port = &serial_ports[port];
    ret = FAILED;

    if(serial_init(port) == OK)
    {
        if((config->parity != SERIAL_PARITY_NONE) && (config->data_bits == SERIAL_DATA_BITS_8))
        {
            word_length = USART_WORDLENGTH_9B;
        }
        else
        {
            word_length = USART_WORDLENGTH_8B;
        }

        if(config->stop_bits == SERIAL_STOP_BITS_1)
        {
            stop_bits = USART_STOPBITS_1;
        }
        else
        {
            stop_bits = USART_STOPBITS_2;
        }

        if(usart_hal_setup(p_port->usart, config->baudrate, word_length, 
                                stop_bits, config->parity, USART_MODE_TX_RX) == OK)
        {
            memcpy(&p_port->config, config, sizeof(serial_config_t));
            usart_hal_receive_dma_toidle(p_port->usart, (uint8_t*) &p_port->rx_queue[0], 
                                                                        p_port->rx_queue_size);
            ret = OK;
        }
    }

    return ret;
}

error_t serial_get_param(uint8_t port, serial_config_t* param)
{
    serial_t* p_port;
    error_t ret;

    ASSERT(SERIAL_IS_PORT(port));
    ASSERT(param != NULL);
    
    p_port = &serial_ports[port];
    ret = FAILED;

    if(p_port->usart)
    {
        memcpy(param, &p_port->config, sizeof(serial_config_t));
        ret = OK;
    }

    return ret;
}

error_t serial_tx(uint8_t port, const uint8_t* pdata, uint16_t buffer_size, uint16_t* p_sent)
{
    uint16_t to_copy;
    uint16_t temp;
    uint16_t available;
    serial_t* p_serial;

    ASSERT(SERIAL_IS_PORT(port));
    ASSERT(pdata != NULL);
    ASSERT(buffer_size > 0);

    p_serial = &serial_ports[port];
    available = p_serial->tx_queue_size - p_serial->tx_queue_cnt;

    to_copy = (buffer_size < available) ? buffer_size : available;

    if(p_sent)
    {
        *p_sent = to_copy;
    }

    if(to_copy > 0)
    {
        if((p_serial->tx_tail + to_copy) > p_serial->tx_queue_size)
        {
            temp = p_serial->tx_queue_size - p_serial->tx_tail;
            memcpy((void*) &p_serial->tx_queue[p_serial->tx_tail], (void*) pdata, temp);

            pdata += temp;
            temp = to_copy - temp;
            memcpy((void*)&p_serial->tx_queue[0], (void*) pdata, temp);
        }
        else
        {
            memcpy((void*)&p_serial->tx_queue[p_serial->tx_tail], (void*) pdata, to_copy);
        }

        p_serial->tx_tail += to_copy;
        if(p_serial->tx_tail >= p_serial->tx_queue_size)
        {
            p_serial->tx_tail -= p_serial->tx_queue_size;
        }

        /*
        * TODO: Enter critical section
        */

        p_serial->tx_queue_cnt += to_copy; 

        /*
        * TODO: Exit critical section 
        */

        if(p_serial->tx_status == SERIAL_TX_STATUS_IDLE)
        {
            if((p_serial->tx_head + p_serial->tx_queue_cnt) > p_serial->tx_queue_size)
            {
                p_serial->tx_xfer_size = p_serial->tx_queue_size - p_serial->tx_head;
            }
            else
            {
                p_serial->tx_xfer_size = p_serial->tx_queue_cnt;
            }

            p_serial->tx_status = SERIAL_TX_STATUS_BUSY;
            
            usart_hal_transmit_dma(p_serial->usart,
                            (const uint8_t*) &p_serial->tx_queue[p_serial->tx_head], 
                            p_serial->tx_xfer_size);
        }
    }

    return OK;
}

error_t serial_rx(uint8_t port, uint8_t* pdata, uint16_t buffersize, uint16_t* nread)
{
    serial_t* p_serial;
    uint16_t to_copy;
    uint16_t rx_count;
    uint16_t rx_overwritten;
    uint16_t contiguous;

    ASSERT(SERIAL_IS_PORT(port));
    ASSERT(pdata != NULL);
    ASSERT(buffersize > 0);

    p_serial = &serial_ports[port];
    rx_count = p_serial->rx_queue_cnt;
    rx_overwritten = 0;

    if(rx_count > p_serial->rx_queue_size)
    {
        rx_overwritten = rx_count - p_serial->rx_queue_size;
        rx_count = p_serial->rx_queue_size;

        p_serial->rx_head += rx_overwritten;
        if(p_serial->rx_head >= p_serial->rx_queue_size)
        {
            p_serial->rx_head -= p_serial->rx_queue_size;
        }
    }

    to_copy = (buffersize < rx_count) ? buffersize : rx_count;

    if(nread)
    {
        *nread = to_copy;
    }

    if(to_copy > 0)
    {
        contiguous = p_serial->rx_queue_size - p_serial->rx_head;
        if(p_serial->config.data_bits == SERIAL_DATA_BITS_8)
        {
            if(to_copy > contiguous)
            {
                memcpy((void*) pdata, (void*) &p_serial->rx_queue[p_serial->rx_head], contiguous);
                pdata += contiguous;
                memcpy((void*) pdata, (void*) &p_serial->rx_queue[0], to_copy - contiguous);
            }
            else
            {
                memcpy((void*) pdata, (void*) &p_serial->rx_queue[p_serial->rx_head], to_copy);
            }
        }
        else
        {
            if(to_copy > contiguous)
            {
                for(uint16_t i = 0; i < contiguous; i++)
                {
                    pdata[i] = p_serial->rx_queue[p_serial->rx_head + i] & 0x7F;
                }
                
                pdata += contiguous;

                for(uint16_t i = 0; i < (to_copy - contiguous); i++)
                {
                    pdata[i] = p_serial->rx_queue[i] & 0x7F;
                }
            }
            else
            {
                for(uint16_t i = 0; i < to_copy; i++)
                {
                    pdata[i] = p_serial->rx_queue[p_serial->rx_head + i] & 0x7F;
                }
            }
        }

        p_serial->rx_head += to_copy;
        if(p_serial->rx_head >= p_serial->rx_queue_size)
        {
            p_serial->rx_head -= p_serial->rx_queue_size;
        }

        /* Critical section */
        p_serial->rx_queue_cnt -= (to_copy + rx_overwritten);
    }

    return OK;
}

static void serial_tx_complete_handler(usart_hal_context_t* p_usart)
{
    serial_t* p_serial = &serial_ports[p_usart->port];

    if(!p_serial)
    {
        return;
    }

    p_serial->tx_queue_cnt -= p_serial->tx_xfer_size;
    p_serial->tx_head += p_serial->tx_xfer_size;
    p_serial->tx_xfer_size = 0;

    if(p_serial->tx_head >= p_serial->tx_queue_size)
    {
        p_serial->tx_head -= p_serial->tx_queue_size;
    }

    if(p_serial->tx_queue_cnt > 0)
    {
        if((p_serial->tx_head + p_serial->tx_queue_cnt) > p_serial->tx_queue_size)
        {
            p_serial->tx_xfer_size = p_serial->tx_queue_size - p_serial->tx_head;
        }
        else
        {
            p_serial->tx_xfer_size = p_serial->tx_queue_cnt;
        }

        p_serial->tx_status = SERIAL_TX_STATUS_BUSY;

        usart_hal_transmit_dma(p_usart, 
                    (const uint8_t*) &p_serial->tx_queue[p_serial->tx_head], 
                    p_serial->tx_xfer_size);
    }
    else
    {
        p_serial->tx_status = SERIAL_TX_STATUS_IDLE;
    }
}

static void serial_rx_complete_handler(usart_hal_context_t* p_usart)
{
    uint16_t rx_count;
    serial_t* p_serial = &serial_ports[p_usart->port];

    if(!p_serial)
    {
        return;
    }

    /* Amount of data received since last interrupt(Idle, Half or Complete) */
    rx_count = p_serial->rx_queue_size - p_serial->rx_tail;
    p_serial->rx_queue_cnt += rx_count;

    if(p_serial->rx_queue_cnt > p_serial->rx_queue_size)
    {
        /* Queue has been overflowed */
        p_serial->rx_status |= SERIAL_RX_STATUS_OVERRUN;
    }

    p_serial->rx_tail = 0;
}

static void serial_rx_hlf_complete_handler(usart_hal_context_t* p_usart)
{
    serial_t* p_serial = &serial_ports[p_usart->port];
    uint16_t rx_count;
    uint16_t remaining;

    if(!p_serial)
    {
        return;
    }

    remaining = usart_hal_get_remaining_rx(p_usart);
    rx_count = p_serial->rx_queue_size - remaining - p_serial->rx_tail;
    p_serial->rx_queue_cnt += rx_count;

    if(p_serial->rx_queue_cnt > p_serial->rx_queue_size)
    {
        /* Queue has been overflowed */
        p_serial->rx_status |= SERIAL_RX_STATUS_OVERRUN;
    }

    p_serial->rx_tail += rx_count;
    if(p_serial->rx_tail >= p_serial->rx_queue_size)
    {
        p_serial->rx_tail -= p_serial->rx_queue_size;
    }
}

static void serial_rx_idle_handler(usart_hal_context_t* p_usart)
{
    serial_t* p_serial = &serial_ports[p_usart->port];
    uint16_t remaining;
    uint16_t rx_count;

    if(!p_serial)
    {
        return;
    }

    remaining = usart_hal_get_remaining_rx(p_usart);
    rx_count = p_serial->rx_queue_size - remaining - p_serial->rx_tail;
    p_serial->rx_queue_cnt += rx_count;

    if(p_serial->rx_queue_cnt > p_serial->rx_queue_size)
    {
        /* Queue has been overflowed */
        p_serial->rx_status |= SERIAL_RX_STATUS_OVERRUN;
    }

    p_serial->rx_tail += rx_count;
    if(p_serial->rx_tail >= p_serial->rx_queue_size)
    {
        p_serial->rx_tail -= p_serial->rx_queue_size;
    }
}