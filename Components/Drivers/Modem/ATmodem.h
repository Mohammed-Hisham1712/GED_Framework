#ifndef __ATMODEM_H__

#define __ATMODEM_H__

#include "serial.h"
#include "timer.h"

#include <stdint.h>

#define ATMODEM_PREFIX      "AT"

#define ATMODEM_CR          0x0D
#define ATMODEM_LF          0x0A

#define ATMODEM_DIAL                "D"
#define ATMODEM_CALL_POSTIFIX       ";"

/**
 * @brief Result codes sent by DCE when ATV1 is used
 * Format: \cr\lf<RESCODE>\cr\lf
 * @note \cr Refers to the character specified by S3, likewise, 
 *          \lf refers to the character specified by S4
 * 
 */
#define ATMODEM_ANS_OK              "OK"
#define ATMODEM_ANS_ERROR           "ERROR"
#define ATMODEM_ANS_CONNECT         "CONNECT"
#define ATMODEM_ANS_NO_CARRIER      "NO CARRIER"
#define ATMODEM_ANS_NO_ANSWER       "NO ANSWER"
#define ATMODEM_ANS_BUSY            "BUSY"
#define ATMODEM_ANS_NO_DIALTONE     "NO DIALTONE"
#define ATMODEM_ANS_RING            "RING"
#define ATMODEM_ANS_CME_ERROR       "+CME ERROR: "

/**
 * @brief Result codes sent by DCE when ATV0 is used
 * Format: <RESCODE>\cr
 * @note \cr Refers to the character specified by S3, likewise, 
 *          \lf refers to the character specified by S4
 *     
 */
#define ATMODEM_ANS_OK_V0           "0"
#define ATMODEM_ANS_CONNECT_V0      "1"
#define ATMODEM_ANS_RING_V0         "2"
#define ATMODEM_ANS_NO_CARRIER_V0   "3"
#define ATMODEM_ANS_ERROR_V0        "4"
#define ATMODEM_ANS_NO_DIALTONE_V0  "6"
#define ATMODEM_ANS_BUSY_V0         "7"
#define ATMODEM_ANS_NO_ANSWER_V0    "8"

#define ATMODEM_RECV_BUFFER_SIZE        64

#define ATMODEM_TEL_NUMBER_MAX_SIZE     14

#define ATMODEM_STATUS_CMD_BUSY             0x01
#define ATMODEM_STATUS_DIALING              0x02
#define ATMODEM_STATUS_VOICE_CALL           0x04
#define ATMODEM_STATUS_HANGUP_IN_PROGRESS   0x08
#define ATMODEM_STATUS_CONNECTED            0x10


typedef enum
{
    ATMODEM_RESCODE_NONE,
    ATMODEM_RESCODE_OK,
    ATMODEM_RESCODE_ERROR,
    ATMODEM_RESCODE_CONNECT,
    ATMODEM_RESCODE_NO_CARRIER,
    ATMODEM_RESCODE_NO_ANSWER,
    ATMODEM_RESCODE_BUSY,
    ATMODEM_RESCODE_NO_DIALTONE,
    ATMODEM_RESCODE_RING,
    ATMODEM_RESCODE_CME_ERROR,
    ATMODEM_RESCODE_NOT_RECOGNIZED,
} atmodem_rescode_t;

typedef enum
{
    ATMODEM_RX_STATE_HEADER_CR,
    ATMODEM_RX_STATE_HEADER_LF,
    ATMODEM_RX_STATE_BODY,
    ATMODEM_RX_STATE_TRAILER_CR,
    ATMODEM_RX_STATE_TRAILER_LF
} atmodem_rx_state_t;

typedef enum
{
    ATMODEM_STATE_CMD_OFFLINE,
    ATMODEM_STATE_CMD_ONLINE,
    ATMODEM_STATE_DATE_ONLINE,
} atmodem_state_t;

typedef enum
{
    ATMODEM_RETVAL_SUCCESS,
    ATMODEM_RETVAL_FAILED,
    ATMODEM_RETVAL_BUSY,
    ATMODEM_RETVAL_WRONG_STATE,
    ATMODEM_RETVAL_UNEXPECTED_RESP,
    ATMODEM_RETVAL_FULL_BUFFER,
    ATMODEM_RETVAL_TX_FAILED,
} atmodem_retval_t;

enum
{
    ATMODEM_MODEM_GSM = 0,
    ATMODEM_MODEM_MAX,
};

typedef error_t (*atmodem_cmd_resp_callback_t)  (atmodem_rescode_t, const char*, uint8_t);

typedef struct
{
    uint8_t                     rx_buffer[ATMODEM_RECV_BUFFER_SIZE];
    atmodem_cmd_resp_callback_t cmd_callback;
    ms_timer_t                  cmd_timer;
    uint16_t                    cmd_timeout;
    uint16_t                    rx_consumed;
    uint8_t                     status;
    uint8_t                     state;
    uint8_t                     rx_state;
    uint8_t                     serial_port;
} atmodem_layer_t;

typedef struct
{
    uint8_t serial_port;
    serial_config_t serial_config;
} atmodem_config_t;

/**
 * @brief 
 * cmd: Pointer to a string containing the command to send to DCE
 * resp_callback: Callback function when info. text or result code
 * is received from DCE in response to the preceeded command. Could
 * NULL
 * timeout_ms: Timeout in milliseconds that DCE should respond within
 * that time
 * cmd_size: Command size in bytes excluding the null byte character. If 0
 * then size of command is calculated by strlen.
 */
typedef struct
{
    const char*                 cmd;
    atmodem_cmd_resp_callback_t resp_callback;
    uint16_t                    timeout_ms;
    uint16_t                    cmd_size;
} atmodem_cmd_desc_t;



/**
 * @brief Send an AT command to DCE according to V.25ter specification
 *        Command can be in basic, S-parameter or extended format.
 *        The function will add the prefix 'AT' as well as the trailer '\cr'.
 *        Example: cmd_desc.cmd = "+CGMI"
 *
 * @param p_modem Pointer to ATmodem layer
 * @param p_cmd_desc Pointer to atmodem_cmd_desc_t that contains the command string,
 *                   callback function to be called upon receiving a response from DCE
 *                   and a timeout in milliseconds for receiving the response
 * @return atmodem_retval_t 
 */
atmodem_retval_t atmodem_send_command(atmodem_layer_t* p_modem, 
                                                const atmodem_cmd_desc_t* p_cmd_desc);


#endif