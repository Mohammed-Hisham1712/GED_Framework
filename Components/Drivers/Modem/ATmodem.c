#include "ATmodem.h"
#include "serial.h"
#include "timer.h"
#include "log.h"
#include "assert.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#define ATMODEM_IS_MODEM(modem)     ((modem) < ATMODEM_MODEM_MAX)

#define ATMODEM_TAG                 "ATMODEM"

#ifndef ATMODEM_LOG_VERBOSITY_SUPRESS
#define ATMODEM_LOGW(msg) \
                        LOGW(ATMODEM_TAG, "[LINE] %d: %s", __LINE__, (msg))
#define ATMODEM_LOGE(msg) \
                        LOGE(ATMODEM_TAG, "[LINE] %d: %s", __LINE__, (msg))
#else
#define ATMODEM_LOGW(msg)   LOGW(ATMODEM_TAG, "%s", (msg))
#define ATMODEM_LOGE(msg)   LOGE(ATMODEM_TAG, "%s", (msg))
#endif  /* ATMODEM_LOG_VERBOSITY_SUPRESS */

#define ATMODEM_CMD_MAX_SIZE        64
#define ATMODEM_RESP_MAX_SIZE       32


#define INTEROCTET_RESP_TIMEOUT     100

/**
 * @brief Convert the string recieved as a response result code to a 
 * numeric integer represnting the result code
 * 
 * @param ans A null terminated string representing the received result code
 * @param ans_size Size of the received string including the null byte
 * @return atmodem_rescode_t Numeric result code corresponding the received string
 */
static atmodem_rescode_t atmodem_get_rescode(const char* ans, uint8_t ans_size)
{
    atmodem_rescode_t rescode;

    ASSERT(ans);

    if(((sizeof(ATMODEM_ANS_OK)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_OK, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_OK;
        LOGD(ATMODEM_TAG, "RESCODE_OK");
    }
    else if(((sizeof(ATMODEM_ANS_ERROR)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_ERROR, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_ERROR;
        LOGD(ATMODEM_TAG, "RESCODE_ERROR");
    }
    else if(((sizeof(ATMODEM_ANS_NO_CARRIER)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_NO_CARRIER, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_NO_CARRIER;
        LOGD(ATMODEM_TAG, "RESCODE_NO_CARRIER");
    }
    else if(((sizeof(ATMODEM_ANS_NO_ANSWER)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_NO_ANSWER, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_NO_ANSWER;
        LOGD(ATMODEM_TAG, "RESCODE_NO_ANSWER");
    }
    else if(((sizeof(ATMODEM_ANS_NO_DIALTONE)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_NO_DIALTONE, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_NO_DIALTONE;
        LOGD(ATMODEM_TAG, "RESCODE_NO_DIALTONE");
    }
    else if(((sizeof(ATMODEM_ANS_BUSY)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_BUSY, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_BUSY;
        LOGD(ATMODEM_TAG, "RESCODE_BUSY");
    }
    else if(((sizeof(ATMODEM_ANS_RING)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_RING, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_RING;
        LOGD(ATMODEM_TAG, "RESCODE_RING");
    }
    else if(((sizeof(ATMODEM_ANS_CME_ERROR)) == ans_size) &&
                                    (memcmp(ATMODEM_ANS_CME_ERROR, ans, ans_size) == 0))
    {
        rescode = ATMODEM_RESCODE_CME_ERROR;
        LOGD(ATMODEM_TAG, "RESCODE_CME_ERROR");
    }
    else
    {
        rescode = ATMODEM_RESCODE_NOT_RECOGNIZED;
        LOGD(ATMODEM_TAG, "RESCODE_NOT_RECOGNIZED");
    }

    return rescode;
}

/**
 * @brief Process response received from DCE according to internal DTE Rx state machine.
 *        Call registerd callback function for the preceeded command.
 *        Update DTE Rx state machine.
 * 
 * @param p_modem Pointer to ATmodem layer
 * @param recv Character received from DCE
 * @return atmodem_retval_t 
 */
static atmodem_retval_t atmodem_process_cmd_resp(atmodem_layer_t* p_modem, char recv)
{
    atmodem_rescode_t rescode;
    atmodem_retval_t l_ret;

    ASSERT(p_modem);

    rescode = ATMODEM_RESCODE_NONE;
    l_ret = ATMODEM_RETVAL_FAILED;

    timer_start(&p_modem->interoctet_resp_timer);

    switch(p_modem->rx_state)
    {
        case ATMODEM_RX_STATE_HEADER_CR:
            if(ATMODEM_CR == recv)
            {
                p_modem->rx_consumed = 0;
                p_modem->rx_state = ATMODEM_RX_STATE_HEADER_LF;
                l_ret = ATMODEM_RETVAL_SUCCESS;
            }
            else
            {
                ATMODEM_LOGW("Wrong header received expected \\cr");
                l_ret = ATMODEM_RETVAL_UNEXPECTED_RESP;
            }
            break;
        case ATMODEM_RX_STATE_HEADER_LF:
            if(ATMODEM_LF == recv)
            {
                p_modem->rx_state = ATMODEM_RX_STATE_BODY;
                l_ret = ATMODEM_RETVAL_SUCCESS;
            }
            else
            {
                ATMODEM_LOGW("Wrong header received expected \\lf");
                l_ret = ATMODEM_RETVAL_UNEXPECTED_RESP;
            }
            break;
        case ATMODEM_RX_STATE_BODY:
            if(ATMODEM_CR == recv)
            {
                p_modem->rx_state = ATMODEM_RX_STATE_TRAILER_LF;
                l_ret = ATMODEM_RETVAL_SUCCESS;
            }
            else if(ATMODEM_LF == recv)
            {
                ATMODEM_LOGW("Wrong trailer received expected \\cr");
                l_ret = ATMODEM_RETVAL_UNEXPECTED_RESP;
            }
            else
            {
                if(p_modem->rx_consumed < (sizeof(p_modem->rx_buffer) - 1))
                {
                    p_modem->rx_buffer[p_modem->rx_consumed] = recv;
                    p_modem->rx_consumed++;
                    l_ret = ATMODEM_RETVAL_SUCCESS;
                }
                else
                {
                    ATMODEM_LOGW("Rx buffer full");
                    l_ret = ATMODEM_RETVAL_FULL_BUFFER;
                }
            }
            break;
        case ATMODEM_RX_STATE_TRAILER_CR:
            break;
        case ATMODEM_RX_STATE_TRAILER_LF:
            if(ATMODEM_LF == recv)
            {
                timer_clear(&p_modem->interoctet_resp_timer);

                p_modem->rx_state = ATMODEM_RX_STATE_HEADER_CR;
                p_modem->rx_buffer[p_modem->rx_consumed++] = '\0';

                LOGI(ATMODEM_TAG, "[RECV] %s", p_modem->rx_buffer);

                rescode = atmodem_get_rescode((const char*) p_modem->rx_buffer, 
                                                            p_modem->rx_consumed);
                
                if(p_modem->status & ATMODEM_STATUS_CMD_BUSY)
                {
                    if(p_modem->cmd_callback)
                    {
                        if(p_modem->cmd_callback(rescode, 
                            (const char*)p_modem->rx_buffer, p_modem->cmd_args) != OK)
                        {
                            ATMODEM_LOGE("Response to cmd failed!");
                        }
                    }
                }
                else
                {
                    /*
                    * TODO: Handle unsco rescode
                    */
                }

                switch(rescode)
                {
                    case ATMODEM_RESCODE_OK:
                    case ATMODEM_RESCODE_ERROR:
                    case ATMODEM_RESCODE_CME_ERROR:
                        p_modem->status &= ~ATMODEM_STATUS_CMD_BUSY;
                        if(p_modem->cmd_semaphore)
                        {
                            xSemaphoreGive(p_modem->cmd_semaphore);
                        }
                        timer_clear(&p_modem->cmd_timer);
                        break;
                    default:
                        break;
                }

                l_ret = ATMODEM_RETVAL_SUCCESS;
            }
            else
            {
                ATMODEM_LOGW("Wrong header received expected \\lf");
                l_ret = ATMODEM_RETVAL_UNEXPECTED_RESP;
            }
            break;
        default:
            p_modem->rx_state = ATMODEM_RX_STATE_HEADER_CR;
            l_ret = ATMODEM_RETVAL_FAILED;
            break;
    }

    return l_ret;
}

/**
 * @brief 
 * 
 * @param p_modem 
 * @param buf 
 * @param buf_size 
 * @return atmodem_retval_t 
 */
static atmodem_retval_t atmodem_tx(atmodem_layer_t* p_modem, 
                                                const uint8_t* buf, uint16_t buf_size)
{
    const uint8_t* p_dest;
    uint16_t to_copy;
    uint16_t n_transmitted;

    ASSERT(p_modem);
    ASSERT(buf);

    p_dest = &buf[0];
    to_copy = buf_size;

    while(to_copy > 0)
    {
        serial_tx(p_modem->serial_port, p_dest, to_copy, &n_transmitted);
        
        to_copy -= n_transmitted;
        p_dest += n_transmitted;
    }

    return ATMODEM_RETVAL_SUCCESS;
}

/**
 * @brief 
 * 
 * @param p_modem 
 * @return atmodem_retval_t 
 */
static atmodem_retval_t atmodem_rx(atmodem_layer_t* p_modem)
{
    uint8_t rx_buf[ATMODEM_RESP_MAX_SIZE];
    uint16_t n_received;
    uint16_t i;

    ASSERT(p_modem);

    do
    {
        serial_rx(p_modem->serial_port, rx_buf, sizeof(rx_buf), &n_received);

        for(i = 0; i < n_received; i++)
        {
            if(atmodem_process_cmd_resp(p_modem, rx_buf[i]) != ATMODEM_RETVAL_SUCCESS)
            {
                return ATMODEM_RETVAL_FAILED;
            }
        }

    } while(n_received);
    
    return ATMODEM_RETVAL_SUCCESS;
}

error_t atmodem_abort_command(atmodem_layer_t* p_modem)
{
    ASSERT(p_modem);

    timer_clear(&p_modem->interoctet_resp_timer);
    timer_clear(&p_modem->cmd_timer);
                    
    p_modem->rx_state = ATMODEM_RX_STATE_HEADER_CR;
    p_modem->status &= ~ATMODEM_STATUS_CMD_BUSY;

    if(p_modem->cmd_semaphore)
    {
        xSemaphoreGive(p_modem->cmd_semaphore);
    }

    return ATMODEM_RETVAL_SUCCESS;
}

/**
 * @brief 
 * 
 * @param args 
 */
static void atmodem_tsk_handler(void* args)
{
    atmodem_layer_t* p_modem;

    ASSERT(args);

    p_modem = (atmodem_layer_t*) args;

    while(1)
    {
        switch(p_modem->state)
        {
            case ATMODEM_STATE_CMD_OFFLINE:
                if(timer_elapsed(&p_modem->interoctet_resp_timer) > 
                                                    INTEROCTET_RESP_TIMEOUT)
                {
                    atmodem_abort_command(p_modem);
                    ATMODEM_LOGW("Inter-octet timer timeout");
                }

                if((p_modem->cmd_timeout > 0) && 
                            timer_elapsed(&p_modem->cmd_timer) > p_modem->cmd_timeout)
                {
                    atmodem_abort_command(p_modem);
                    ATMODEM_LOGW("CMD response timed out");
                }

                atmodem_rx(p_modem);
                break;
            case ATMODEM_STATE_CMD_ONLINE:
                break;
            case ATMODEM_STATE_DATE_ONLINE:
                break;
            default:
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(p_modem->tsk_period));
    }

    vTaskDelete(NULL);
}

atmodem_retval_t atmodem_init(atmodem_layer_t* p_modem, const atmodem_config_t* p_config)
{
    BaseType_t ret;

    ASSERT(p_modem);
    ASSERT(p_config);

    memset((void*) p_modem, 0, sizeof(atmodem_layer_t));

    if(serial_setup(p_config->serial_port, &p_config->serial_config) != OK)
    {
        ATMODEM_LOGE("Init failed");
        return ATMODEM_RETVAL_FAILED;
    }

    p_modem->serial_port = p_config->serial_port;
    p_modem->tsk_period = p_config->tsk_period;
    vSemaphoreCreateBinary(p_modem->cmd_semaphore);
    
    if(p_modem->cmd_semaphore == NULL)
    {
        return ATMODEM_RETVAL_FAILED;
    }
    
    ret = xTaskCreate(atmodem_tsk_handler, "atmodem_task", 
                                        p_config->tsk_stack_size,
                                        p_modem,
                                        p_config->tsk_priority,
                                        &p_modem->tsk_handle);
    if(ret != pdPASS)
    {
        return ATMODEM_RETVAL_FAILED;
    }

    LOGD(ATMODEM_TAG, "Init done");

    return ATMODEM_RETVAL_SUCCESS;
}

atmodem_retval_t atmodem_deinit(atmodem_layer_t* p_modem)
{
    vTaskDelete(p_modem->tsk_handle);
    
    if(p_modem->cmd_semaphore)
    {
        vSemaphoreDelete(p_modem->cmd_semaphore);
    }

    return ATMODEM_RETVAL_SUCCESS;
}

atmodem_retval_t atmodem_send_command(atmodem_layer_t* p_modem, 
                                                const atmodem_cmd_desc_t* p_cmd_desc)
{
    char at_cmd[ATMODEM_CMD_MAX_SIZE];
    char* p_dest;
    uint16_t cmd_size;
    uint16_t to_send;

    ASSERT(p_modem);
    ASSERT(p_cmd_desc);
    ASSERT(p_cmd_desc->cmd);

    p_dest = &at_cmd[0];

    if(p_modem->state != ATMODEM_STATE_CMD_OFFLINE)
    {
        ATMODEM_LOGE("DCE is not in command state");
        return ATMODEM_RETVAL_WRONG_STATE;
    }
    if(p_modem->status & ATMODEM_STATUS_CMD_BUSY)
    {
        ATMODEM_LOGE("Another command is in progress");
        return ATMODEM_RETVAL_BUSY;
    }

    if(p_cmd_desc->cmd_size > 0)
    {
        cmd_size = p_cmd_desc->cmd_size;
    }
    else
    {
        cmd_size = strlen(p_cmd_desc->cmd);
    }

    if((sizeof(ATMODEM_PREFIX) + cmd_size) < sizeof(at_cmd))
    {
        memcpy(at_cmd, ATMODEM_PREFIX, sizeof(ATMODEM_PREFIX) - 1);
        p_dest += sizeof(ATMODEM_PREFIX) - 1;

        memcpy(p_dest, p_cmd_desc->cmd, cmd_size);
        p_dest += cmd_size;

        *p_dest++ = ATMODEM_CR;
        *p_dest = '\0'; /* For debugging */
    }
    else
    {
        ATMODEM_LOGE("Command is too long to fit in buffer");
        return ATMODEM_RETVAL_FULL_BUFFER;
    }
    
    p_modem->cmd_callback = p_cmd_desc->resp_callback;
    p_modem->cmd_args = p_cmd_desc->args;
    p_modem->cmd_timeout = p_cmd_desc->timeout_ms;
    p_modem->status |= ATMODEM_STATUS_CMD_BUSY;
    to_send = p_dest - at_cmd;

    LOGI(ATMODEM_TAG, "[SENDING] %s", at_cmd);

    if(atmodem_tx(p_modem, (const uint8_t*) at_cmd, to_send) != ATMODEM_RETVAL_SUCCESS)
    {
        ATMODEM_LOGE("Tx failed");
        return ATMODEM_RETVAL_TX_FAILED;
    }

    if(p_modem->cmd_timeout > 0)
    {
        timer_start(&p_modem->cmd_timer);
    }

    return ATMODEM_RETVAL_SUCCESS;
}

atmodem_retval_t atmodem_send_command_wait(atmodem_layer_t* p_modem, 
                                    const atmodem_cmd_desc_t* p_cmd, uint32_t timeout)
{
    BaseType_t ret;

    if(p_modem->cmd_semaphore)
    {
        ret = xSemaphoreTake(p_modem->cmd_semaphore, timeout);

        if(ret != pdTRUE)
        {
            ATMODEM_LOGW("Failed to take semaphore");
        }
    }
    else
    {
        return ATMODEM_RETVAL_FAILED;
    }
    
    return atmodem_send_command(p_modem, p_cmd);
}

