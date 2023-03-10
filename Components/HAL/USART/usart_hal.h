#ifndef __USART_HAL_H__

#define __USART_HAL_H__

#include "usart_types.h"

#include "types.h"

#include <stdint.h>
#include <stddef.h>

/* PUBLIC FUNCTION DECLARATIONS */

/**
 * @brief Initializes USART peripheral for the specificed USART instance. Initialization
 *        includes enabling the USART clock, put GPIO pins in AF mode and setting up NVIC IRQs
 * 
 * @param usart_id USART instance ID to initialize. Should be one of (USART1,2..6) 
 * @return usart_hal_context_t* Pointer to a structure that contains all USART attributes
 *          and parameters. This pointer is used in subsequent function calls and callbacks.
 */
usart_hal_context_t* usart_hal_init(uint8_t usart_id);

/**
 * @brief Set UART communication parameters, calculate baudrate generator values and enable
 *        Tx, Rx or both and UART periphera. An Idle frame is sent when this function is 
 *        called
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param baudrate Baudrate
 * @param wordlength Number of data bits including parity bit if parity is enabled
 * @param stopbits Number of stop bits (1 or 2)
 * @param parity Parity can be None, Even or Odd. Number of bits specificed in wordlength
 *               includes this parity bit too
 * @return error_t 
 */
error_t usart_hal_setup(usart_hal_context_t* usart, uint32_t baudrate,
                    uint8_t wordlength, uint8_t stopbits, uint8_t parity, uint8_t mode);

/**
 * @brief Initialze the DMA Tx channel for the specified UART instance and set mode and data
 *        size. This function should be called before using usart_hal_transmit_dma()
 * @warning In case of 9 data bits with no parity bit, DMA mode CANNOT be used
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param dma_circular 1: DMA in circular mode, 0: DMA in normal mode
 * @return error_t 
 */
error_t usart_hal_tx_dma_setup(usart_hal_context_t* usart, uint8_t dma_circular);

/**
 * @brief Initialze the DMA Rx channel for the specified UART instance and set mode and data
 *        size. This function should be called before using usart_hal_receive_dma() or
 *        usart_hal_receive_dma_toidle()
 * @warning In case of 9 data bits with no parity bit, DMA mode CANNOT be used.
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param dma_circular 1: DMA in circular mode, 0: DMA in normal mode
 * @return error_t 
 */
error_t usart_hal_rx_dma_setup(usart_hal_context_t* usart, uint8_t dma_circular);

/**
 * @brief Enable UART transmitter. An Idle frame is sent when called
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return error_t 
 */
error_t usart_hal_enable_tx(usart_hal_context_t* usart);

/**
 * @brief Disable UART transmitter. Any data yet to be transmitted won't be sent.
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return error_t 
 */
error_t usart_hal_disable_tx(usart_hal_context_t* usart);

/**
 * @brief Enable UART receiver. You should call one of the receiving functions to actually
 *        receive data sent to Rx
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return error_t 
 */
error_t usart_hal_enable_rx(usart_hal_context_t* usart);

/**
 * @brief Disable UART transmitter. Any data yet to be received will be lost
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return error_t 
 */
error_t usart_hal_disable_rx(usart_hal_context_t* usart);

/**
 * @brief Register functions to be called every time one of the following events occur
 *        1) Tx has transmitted all the specified data in usart_hal_transmit_it or
 *              usart_hal_transmit_dma()
 *        2) Rx has received the amount of data specified when usart_hal_receive_it() or
 *              usart_hal_receive_dma() are called. Also applies for the toidle variations
 *        3) Rx has received an IDLE frame when receiving data using usart_hal_receive_it_toidle()
 *              or usart_hal_receive_dma_toidle()
 *        4) when a UART error occurs during reception or DMA error during Tx/Rx transfer
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param id Callback type to register this function to be called upon
 * @param p_callback Pointer to the callback function
 * @return error_t 
 */
error_t usart_hal_register_callback(usart_hal_context_t* usart, usart_callback_id_t id,
                                                void (*p_callback)(usart_hal_context_t*));

/**
 * @brief Send data in polling/blocking mode (8-bit & 9-bit modes)
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pdata Pointer to data to be sent
 * @param size Size of data units to be sent. In case of 9-bit with not parity bit, 
 *              size is the number of 9-bit data items, uint16_t, not bytes
 * @return error_t 
 */
error_t usart_hal_transmit(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t size);

/**
 * @brief Send data in interrupt, non-blocking, mode(8/9 bit data are allowed). When all data
 *        items are sent and TC(Transfer Complete) flag is set, USART_TX_COMPLETE_CALLBACK is
 *        called if a callback function has been registered.
 * @note  In Tx interrupt mode, no errors can occur
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pdata Pointer to data to be sent
 * @param sz Size of data units to be sent. In case of 9-bit with not parity bit, 
 *              size is the number of 9-bit data items, uint16_t, not bytes
 * @return error_t 
 */
error_t usart_hal_transmit_it(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t sz);

/**
 * @brief Send data in DMA, non-blocking, mode(8-bit only). When all data items are
 *        sent and TC(Transfer Complete) flag is set, USART_TX_COMPLETE_CALLBACK is
 *        called if a callback function has been registered.
 * @note  In DMA circular mode, after all data transmitted another transfer starts immediately
 *        and TC flag is never set, in this case tx_complete_callback() is called after DMA
 *        Transfer Complete not after USART TC=1.
 * @note  Tx Half transfer callback is not supported yet!
 * @note  In Tx DMA, only USART_DMA_ERROR can occur. In this case transfer is aborted, and
 *        the number of remaining items can be obtained by usart_hal_get_remaining_tx(). Also,
 *        USART_ERROR_CALLBACK is called if provided.
 * @warning In DMA mode 9-bit data with no parity is not supported yet!
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pdata Pointer to data to be sent
 * @param sz    Size of data to be sent in bytes
 * @return error_t 
 */
error_t usart_hal_transmit_dma(usart_hal_context_t* usart, const uint8_t* pdata, uint16_t sz);

/**
 * @brief Receive data in polling, blocking, mode. 8/9 bit data bits are supported.
 * 
 * @param usart   Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer Pointer to buffer where received data are stored
 * @param size    Size of that buffer. For 9-bit data with no parity, size specifies the
 *                number of items to be received not the amount of bytes
 * 
 * @return error_t 
 * 
 * @note In case of Framing, Noise, Parity or Overrun error, the corresponding error flag is
 *       raised and can be obtained by usart_hal_get_error(). Also, the number of items
 *       received is stored and can be obtained by usart_hal_get_received_count()
 * @note Analyzing performance @16MHz
 * Tr = 17.6uS
 * Tr: Time between processsing a received byte and polling for another byte.
 *      This time is measured when RXNE=1 until polling for RXNE again.
 * Theoritical max. baudrate = 10 * (1/Tr) = 568200 bauds. Stopbits = 1, Databits = 8
 * Max. baudrate = 460800 bauds (Max. standard baudrate that could be handled @16MHz)
 * Test baudrates: 19200, 115200 and 460800
 */
error_t usart_hal_receive(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Receive data in polling, blocking, mode. 8/9 bit data bits are supported. Return if
 *        IDLE frame is received.
 * 
 * @param usart   Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer Pointer to buffer where received data are stored.
 * @param size    Size of that buffer. For 9-bit data with no parity, size specifies the
 *                number of items to be received not the amount of bytes.
 * 
 * @return error_t
 * 
 * @note This functions returns OK upon receiving and Idle frame, the number of data received
 *       before IDLE frame can be obtained by usart_hal_get_received_count().
 */
error_t usart_hal_receive_toidle(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Receive data in interrupt, non-blocking mode. When all specifiec bytes has been
 *        received, USART_RX_COMPLETE_CALLBACK is called if provided. In case of Framing,
 *        Noise, Parity or Overrun errors, USART_ERROR_CALLBACK is called if provided.
 * 
 * @param usart     Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer   Pointer to buffer where received data are stored.
 * @param size      Size of that buffer. For 9-bit data with no parity, size specifies the
 *                  number of items to be received not the amount of bytes.
 * @return error_t 
 * 
 * @note Analyzing performance @16MHz
 * Execution time Te = 27uS
 */
error_t usart_hal_receive_it(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Receive data in interrupt, non-blocking mode. When all specifiec bytes has been
 *        received, USART_RX_COMPLETE_CALLBACK is called if provided. In case of Framing,
 *        Noise, Parity or Overrun errors, USART_ERROR_CALLBACK is called if provided.
 *        If an IDLE frame was received, USART_RX_IDLE_RECEIVED_CALLBACK is called if provided
 *        and receive is aborted. Call usart_hal_get_received_count() to get number of received
 *        items
 * 
 * @param usart     Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer   Pointer to buffer where received data are stored.
 * @param size      Size of that buffer. For 9-bit data with no parity, size specifies the
 *                  number of items to be received not the amount of bytes.
 * @return error_t 
 */
error_t usart_hal_receive_it_toidle(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Receive data in DMA, non-blocking mode. In case of circular mode DMA, once all
 *        specified bytes has been received, DMA starts over again from the start of the
 *        buffer. Care must be taken to process received data before being overwiteen by
 *        another DMA transfer.
 * 
 * @param usart     Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer   Pointer to buffer where received data are stored.
 * @param size      Size of that buffer. For 9-bit data with no parity, size specifies the
 *                  number of items to be received not the amount of bytes.
 * @return error_t
 * 
 * @note When Half of the items specified is received, USART_RX_HALF_COMPLETE_CALLBACK is called
 *       if provided.
 *       When all items has been received, USART_RX_COMPLETE_CALLBACK is called if provided.
 *       In case of a USART error or DMA error, transfer is aborted and USART_ERROR_CALLBACK
 *       is called. Number of received items can be obtained through 
 *       usart_hal_get_received_count()
 * @warning In DMA mode 9-bit data with no parity is not supported yet!
 * 
 * @note Analyzing performance @16MHz
 * Exec. time Te = 81uS 
 */
error_t usart_hal_receive_dma(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Receive data in DMA, non-blocking mode. In case of circular mode DMA, once all
 *        specified bytes has been received, DMA starts over again from the start of the
 *        buffer. Care must be taken to process received data before being overwiteen by
 *        another DMA transfer. If and Idle frame is received USART_RX_IDLE_RECEIVED_CALLBACK 
 *        is called. In non-circular mode, current DMA transfer is aborted in case of IDLE frame
 *        In DMA circular mode, if an Idle frame is received transfer is not aborted and further
 *        data can be received. In either case, number of received items before the IDLE frame
 *        are obtained by calling usart_hal_get_received_data()
 * 
 * @param usart     Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @param pbuffer   Pointer to buffer where received data are stored.
 * @param size      Size of that buffer. For 9-bit data with no parity, size specifies the
 *                  number of items to be received not the amount of bytes.
 * @return error_t 
 * 
 * @note When Half of the items specified is received, USART_RX_HALF_COMPLETE_CALLBACK is called
 *       if provided.
 *       When all items has been received, USART_RX_COMPLETE_CALLBACK is called if provided.
 *       In case of a USART error or DMA error, transfer is aborted and USART_ERROR_CALLBACK
 *       is called. Number of received items can be obtained through 
 *       usart_hal_get_received_count()
 * @warning In DMA mode 9-bit data with no parity is not supported yet!
 * 
 */
error_t usart_hal_receive_dma_toidle(usart_hal_context_t* usart, uint8_t* pbuffer, uint16_t size);

/**
 * @brief Get USART error code in case of Framing, Noise, Parity, Overrun or DMA erros
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return uint16_t Error code
 */
uint16_t usart_hal_get_error(usart_hal_context_t* usart);

/**
 * @brief Get number of remaining items to be received
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return uint16_t Remaining items
 */
uint16_t usart_hal_get_remaining_rx(usart_hal_context_t* usart);

/**
 * @brief Get number of remaining items to be sent
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return uint16_t Remaining items
 */
uint16_t usart_hal_get_remaining_tx(usart_hal_context_t* usart);

/**
 * @brief Get number of transmitted data so far. This can be used if a DMA error has occured
 *          to get the number of tranmsitted data before the error
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return uint16_t Tranmitted data
 */
uint16_t usart_hal_get_transmitted_count(usart_hal_context_t* usart);

/**
 * @brief Get number of received data so far. This can be used if a DMA error has occured
 *          to get the number of received data before the error or in case of IDLE frame
 *          reception
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * @return uint16_t Received data
 */
uint16_t usart_hal_get_received_count(usart_hal_context_t* usart);

/**
 * @brief Handler for USART IRQs
 * 
 * @param usart Pointer to usart_hal_context_t that was given by usart_hal_init()
 * 
 * @note Analyzing performance @16MHz
 * For RXNE interrupt
 *      Exec. time Te = 48.2uS
 *      Theoritical max. baudrate = 10 * 1/Te = 207.46Kbps
 *      @9600 bauds and frequent interruption, 4.65% of CPU time is spent processing the IRQ.
 *      @19200 bauds ~~~~~~~~~~~~~~~~~~~~~~~~, 9.25% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.
 *      @115200 bauds ~~~~~~~~~~~~~~~~~~~~~~~, 55.5% ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.
 *      @230400 bauds Reception failed due to Overrun error!
 * 
 * For DMA Rx Complete interrupt (Normal mode)
 *      Exec. time Te(dma_irq_handler + usart_rx_dma_complete) = 72uS
 * For DMA Rx Complete interrupt (Circular mode)
 *      Exec. time Te(dma_irq_handler + usart_rx_dma_complete) = 41uS
 * Te is the time after which the user rx_complete_callback is called
 * In DMA mode max. baudrate depends on the amount of items received in one DMA transfer
 * and how fast the application can process the received items.
 */
void usart_hal_irq_handler(usart_hal_context_t* usart);

#endif