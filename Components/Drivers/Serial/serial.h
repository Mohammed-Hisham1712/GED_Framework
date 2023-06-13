#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "usart_hal.h"
#include "types.h"

#include <stdint.h>
#include <stddef.h>

#define SERIAL_TX_DMA   0x01
#define SERIAL_RX_DMA   0x02
#define SERIAL_TX_INT   0x04
#define SERIAL_RX_INT   0x08
#define SERIAL_TX_POLL  0x10
#define SERIAL_RX_POLL  0x20

#define SERIAL_PORT_MAX     USART_HAL_DEVS

#define SERIAL_TX_QUEUE_SIZE     100
#define SERIAL_RX_QUEUE_SIZE     100

enum
{
    SERIAL_DATA_BITS_7 = 7,
    SERIAL_DATA_BITS_8
};

enum
{
    SERIAL_STOP_BITS_1 = 1,
    SERIAL_STOP_BITS_2
};

enum
{
    SERIAL_PARITY_NONE,
    SERIAL_PARITY_EVEN,
    SERIAL_PARITY_ODD
};


#define SERIAL_TX_STATUS_NONE   0x00
#define SERIAL_TX_STATUS_IDLE   0x01
#define SERIAL_TX_STATUS_BUSY   0x02

#define SERIAL_RX_STATUS_NORMAL     0x00
#define SERIAL_RX_STATUS_OVERRUN    0x01

typedef struct 
{
    uint32_t baudrate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
} serial_config_t;

typedef struct 
{
    usart_hal_context_t* usart;
    serial_config_t config;
    uint16_t tx_xfer_size;
    uint16_t rx_xfer_size;
    uint16_t tx_queue_cnt;
    uint16_t rx_queue_cnt;
    uint16_t tx_queue_size;
    uint16_t rx_queue_size;
    uint16_t tx_head;
    uint16_t tx_tail;
    uint16_t rx_head;
    uint16_t rx_tail;
    uint8_t tx_status;
    uint8_t rx_status;
    volatile uint8_t tx_queue[SERIAL_TX_QUEUE_SIZE];
    volatile uint8_t rx_queue[SERIAL_RX_QUEUE_SIZE];
} serial_t;

/**
 * @brief 
 * 
 * @param port
 * @param param 
 * @return error_t 
 */
error_t serial_setup(uint8_t port, const serial_config_t* param);

/**
 * @brief 
 * 
 * @param port 
 * @param param 
 * @return error_t 
 */
error_t serial_get_param(uint8_t port, serial_config_t* param);

/**
 * @brief 
 * 
 * @param port 
 * @param baudrate 
 * @return error_t 
 */
error_t serial_set_baudrate(uint8_t port, uint32_t baudrate);

/**
 * @brief 
 * 
 * @param port 
 * @return uint32_t 
 */
uint32_t serial_get_baudrate(uint8_t port);

/**
 * @brief 
 * 
 * @param port 
 * @param data_bits 
 * @return error_t 
 */
error_t serial_set_data_bits(uint8_t port, uint8_t data_bits);

/**
 * @brief 
 * 
 * @param port 
 * @return uint8_t 
 */
uint8_t serial_get_data_bits(uint8_t port);

/**
 * @brief 
 * 
 * @param port 
 * @param stop_bits 
 * @return error_t 
 */
error_t serial_set_stop_bits(uint8_t port, uint8_t stop_bits);

/**
 * @brief 
 * 
 * @param port 
 * @return uint8_t 
 */
uint8_t serial_get_stop_bits(uint8_t port);

/**
 * @brief 
 * 
 * @param port 
 * @param parity 
 * @return error_t 
 */
error_t serial_set_parity(uint8_t port, uint8_t parity);

/**
 * @brief 
 * 
 * @param port 
 * @return uint8_t 
 */
uint8_t serial_get_parity(uint8_t port);

/**
 * @brief 
 * 
 * @param port 
 * @param pdata 
 * @param size 
 * @param sent 
 * @return error_t 
 */
error_t serial_tx(uint8_t port, const uint8_t* pdata, uint16_t size, uint16_t* sent);

/**
 * @brief 
 * 
 * @param port 
 * @param pdata 
 * @param buffersize 
 * @param nread 
 * @return error_t 
 */
error_t serial_rx(uint8_t port, uint8_t* pdata, uint16_t buffersize, uint16_t* nread);

#endif