#ifndef __GSM_MODEM_H__

#define __GSM_MODEM_H__


#include "ATmodem.h"

#include <stdint.h>


#define GSM_DCE_MANUFACTURER_MAX_SIZE       32
#define GSM_DCE_MODEL_NAME_MAX_SIZE         32
#define GSM_DCE_REVISION_MAX_SIZE           48
#define GSM_DCE_IMEI_SIZE                   15
#define GSM_DCE_IMSI_SIZE                   15
#define GSM_CELL_LAC_SIZE                   4
#define GSM_CELL_ID_SIZE                    4
#define GSM_OP_LONG_MAX_SIZE                16
#define GSM_OP_SHORT_MAX_SIZE               8
#define GSM_OP_NUMERIC_SIZE                 5

#define GSM_CMD_DEFAULT_TIMEOUT         300
#define GSM_CMD_SET_FUNC_LEVEL_TIMEOUT  10000
#define GSM_CMD_OP_SELECTION_TIMEOUT    3000

/*
 * General commands specified in 3GPP TS 27.007 version 10.3.0 Release 10
*/

#define GSM_CMD_GET_MANUFACTURER        "+CGMI"
#define GSM_CMD_GET_MODEL_NAME          "+CGMM"
#define GSM_CMD_GET_REVISION            "+CGMR"
#define GSM_CMD_GET_IMEI                "+CGSN"
#define GSM_CMD_GET_IMSI                "+CIMI"
#define GSM_CMD_GET_CHSET               "+CSCS?"
#define GSM_CMD_SET_CHSET               "+CSCS=\"%s\""
#define GSM_CMD_GET_SIGNAL_QUALITY      "+CSQ"


/*
* Mobile termination control commands specified in 3GPP 27.007 version 10.3.0 Release 10
*/

#define GSM_CMD_GET_ACTIV_STATUS        "+CPAS"
#define GSM_CMD_GET_FUNC_LEVEL          "+CFUN?"
#define GSM_CMD_SET_FUNC_LEVEL          "+CFUN=%u"
#define GSM_CMD_SOFT_RESET              "+CFUN=1,1"



#define GSM_CMD_CREG_DISABLED           "+CREG=0"
#define GSM_CMD_CREG_ENABLED            "+CREG=1"
#define GSM_CMD_CREG_ENABLED_WITH_LOC   "+CREG=2"

#define GSM_CMD_OP_FORMAT_LONG          "+COPS=3,0"
#define GSM_CMD_OP_FORMAT_SHORT         "+COPS=3,1"
#define GSM_CMD_OP_FORMAT_NUMERIC       "+COPS=3,2"
#define GSM_CMD_OP_SELECTION            "+COPS=%u,%u,\"%s\""
#define GSM_CMD_OP_SELECTION_NO_OP      "+COPS=%u,%u"
#define GSM_CMD_OP_GET                  "+COPS?"


#define GSM_UNSO_CREG                   "+CREG: "


#define GSM_MODEM_STR_CHSET_GSM         "GSM"
#define GSM_MODEM_STR_CHSET_HEX         "HEX"
#define GSM_MODEM_STR_CHSET_IRA         "IRA"
#define GSM_MODEM_STR_CHSET_PCCP        "PCCP"
#define GSM_MODEM_STR_CHSET_PCDN        "PCDN"
#define GSM_MODEM_STR_CHSET_UCS2        "UCS2"
#define GSM_MODEM_STR_CHSET_UTF8        "UTF-8"
#define GSM_MODEM_STR_CHSET_8859_1      "8859-1"
#define GSM_MODEM_STR_CHSET_8859C       "8859-c"

typedef uint8_t gsm_modem_chset_t;
typedef uint8_t gsm_modem_activ_status_t;
typedef uint8_t gsm_modem_func_level_t;

enum
{
    GSM_MODEM_CHSET_GSM = 0,
    GSM_MODEM_CHSET_HEX,
    GSM_MODEM_CHSET_IRA,
    GSM_MODEM_CHSET_PCCP,
    GSM_MODEM_CHSET_PCDN,
    GSM_MODEM_CHSET_UCS2,
    GSM_MODEM_CHSET_UTF8,
    GSM_MODEM_CHSET_8859_1,
    GSM_MODEM_CHSET_8859C,
    GSM_MODEM_CHSET_INVALID,
};

enum
{
    GSM_MODEM_ACTIV_STATUS_READY = 0,
    GSM_MODEM_ACTIV_STATUS_UNAVAILABLE,
    GSM_MODEM_ACTIV_STATUS_UNKOWN,
    GSM_MODEM_ACTIV_STATUS_RINGING,
    GSM_MODEM_ACTIV_STATUS_CALL_IN_PROGRESS,
    GSM_MODEM_ACTIV_STATUS_ASLEEP,
    GSM_MODEM_ACTIV_STATUS_INVALID,
};

enum
{
    GSM_MODEM_FUNC_LEVEL_MIN = 0,
    GSM_MODEM_FUNC_LEVEL_FULL,
    GSM_MODEM_FUNC_LEVEL_TX_DISABLED,
    GSM_MODEM_FUNC_LEVEL_RX_DISABLED,
    GSM_MODEM_FUNC_LEVEL_RF_DISABLED,
    GSM_MODEM_FUNC_LEVEL_INVALID,
};

typedef enum
{
    GSM_MODEM_NOT_REGISTERED_NOT_SEARCHING=0,
    GSM_MODEM_REGISTERED_HOME_NETWORK,
    GSM_MODEM_NOT_REGISTERED_SEARCHING,
    GSM_MODEM_REGISTERATION_DENIED,
    GSM_MODEM_REGISTERATION_UNKOWN,
    GSM_MODEM_REGISTERED_ROAMING,
    GSM_MODEM_REGISTERATION_INVALID,
}gsm_modem_network_registeration_status_t;

enum
{
    GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_DISABLED=0,
    GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_ENABLED,
    GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_WITH_LOC,
};

typedef enum
{
    GSM_MODEM_OP_FORMAT_LONG = 0,
    GSM_MODEM_OP_FORMAT_SHORT,
    GSM_MODEM_OP_FORMAT_NUMERIC,
    GSM_MODEM_OP_FORMAT_INVALID
} gsm_modem_op_format_t;

typedef enum
{
    GSM_MODEM_OP_SELECTION_AUTO = 0,
    GSM_MODEM_OP_SELECTION_MANUAL,
    GSM_MODEM_OP_SELECTION_DEREGISTER,
    GSM_MOEDM_OP_SELECTION_FORMAT,
    GSM_MODEM_OP_SELECTION_AUTO_MANUAL,
    GSM_MODEM_OP_SELECTION_INVALID,
} gsm_modem_op_selection_t;

typedef enum
{
    GSM_MODEM_OP_STATUS_UNKOWN = 0,
    GSM_MODEM_OP_STATUS_AVAILABLE,
    GSM_MODEM_OP_STATUS_CURRENT,
    GSM_MODEM_OP_STATUS_FORBIDDEN,
    GSM_MODEM_OP_STATUS_MAX,
} gsm_modem_op_status_t;



typedef struct
{
    void*                   priv_resources;
    atmodem_layer_t         dte_layer;
} gsm_modem_t;


/**
 * @brief Get DCE manufacturer name by invoking +CGMI command. This command will
 * block the calling task until a response is received from DCE or timeout has
 * elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param manufacturer Pointer to a string big enough to hold manufacturer name. 
 *                     Should be 32 characters or more.
 * @return error_t 
 */
error_t gsm_modem_get_dce_manufacturer(gsm_modem_t* p_gsm_modem, char* manufacturer);


/**
 * @brief Get DCE model name by invoking +CGMM command. This command will
 * block the calling task until a response is received from DCE or timeout has
 * elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param model_name Pointer to a string big enough to hold model name of DCE. 
 *                   Should be 32 characters or more.
 * @return error_t
 */
error_t gsm_modem_get_dce_model_name(gsm_modem_t* p_gsm_modem, char* model_name);


/**
 * @brief Get DCE revision by invoking +CGMR command. This command will
 * block the calling task until a response is received from DCE or timeout has
 * elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param revision Pointer to a string big enough to hold DCE revision. 
 *                 Should be 48 characters or more.
 * @return error_t 
 */
error_t gsm_modem_get_dce_revision(gsm_modem_t* p_gsm_modem, char* revision);

/**
 * @brief Get DCE Internationl Mobile Equipment Identity (IMEI) by 
 *        invoking +CGSN command. This command will block the calling task until 
 *        a response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param imei Pointer to a string big enough to hold IMEI. Should be 16 characters 
 *             or more.
 * @return error_t
 */
error_t gsm_modem_get_dce_imei(gsm_modem_t* p_gsm_modem, char* imei);


/**
 * @brief Get DCE Internationl Mobile Subscriber Identity (IMSI) by 
 *        invoking +CIMI command. This command will block the calling task until 
 *        a response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param imei Pointer to a string big enough to hold IMSI. Should be 16 characters 
 *             or more.
 * @return error_t
 */
error_t gsm_modem_get_dce_imsi(gsm_modem_t* p_gsm_modem, char* imsi);

/**
 * @brief Get current character set used between TE-TA interface by invoking
 *        +CSCS command. This command will block the calling task until 
 *        a response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param chset Pointer to a variable that will hold the response from +CSCS command.
 * 
 * @return error_t 
 */
error_t gsm_modem_get_dce_character_set(gsm_modem_t* p_gsm_modem, 
                                                    gsm_modem_chset_t* chset);

/**
 * @brief Set the character set that shall be used between TE-TA interface.
 *        Settings will affect the reception and transmission of SMS messages
 *        as well as phonebook entries. This command will block the calling task until 
 *        a response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param chset Variable that holds the character set to be applied to TE-TA interface,
 *              i.e, GSM_MODEM_CHSET_GSM or GSM_MODEM_CHSET_UCS2
 * @return error_t 
 */
error_t gsm_modem_set_dce_character_set(gsm_modem_t* p_gsm_modem, 
                                                    gsm_modem_chset_t chset);

/**
 * @brief Convert the character set give in \p chset to a string matching the
 *        the value send in +CSCS="<chest>" or the +CSCS: "<chset>" response
 * 
 * @param chset 
 * @return const char* Pointer to a string representing the character set, or NULL
 *         if \p chset is an invalid value.
 */
const char* gsm_modem_dce_chset_to_str(gsm_modem_chset_t chset);



/**
 * @brief Get MT activity status by executing +CPAS command. Status indicates
 *        if MT can respond to commands or not, it can also indicate if a call
 *        or ringing is active. This command will block the calling task until 
 *        a response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param p_status Pointer to variable to hold the activity status of MT
 * @return error_t 
 */
error_t gsm_modem_get_mt_activ_status(gsm_modem_t* p_gsm_modem, 
                                                gsm_modem_activ_status_t* p_status);

/**
 * @brief Perform a software reset to MT by executing +CFUN=1,1 command. 
 *        This command will block the calling task until a response 
 *        is received from DCE or timeout has elapsed.
 * 
 * @note  Function should be executed only when MT is either in Full or Minimum
 *        functionality.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @return error_t 
 */
error_t gsm_modem_soft_reset(gsm_modem_t* p_gsm_modem);


/**
 * @brief Set the functionality level of MT. Level GSM_MODEM_FUNC_LEVEL_FULL is
 *        where maximum power is consumed, level GSM_MODEM_FUNC_LEVEL_MIN is
 *        where minimum power is consumed. This command will block the calling 
 *        task until a response is received from DCE or timeout has elapsed.
 * 
 * @note Transition between GSM_MODEM_FUNC_LEVEL_MIN and 
 *       GSM_MODEM_FUNC_LEVEL_RF_DISABLED is not allowed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param level 
 * @return error_t 
 */
error_t gsm_modem_set_mt_func_level(gsm_modem_t* p_gsm_modem, 
                                                    gsm_modem_func_level_t level);

/**
 * @brief Get the functionality level of MT by invoking +CFUN? read command.
 *        This command will block the calling task until a response
 *        is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param p_level 
 * @return error_t 
 */
error_t gsm_modem_get_mt_func_level(gsm_modem_t* p_gsm_modem, 
                                                gsm_modem_activ_status_t* p_level);


/**
 * @brief Get received signal strength indication RSSI and channel bit error BER by
 *        executing +CSQ command. This command will block the calling task until a 
 *        response is received from DCE or timeout has elapsed.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t variable
 * @param p_rssi Pointer to uint8_t variable to hold RSSI value
 * @param p_ber Pointer to uint8_t variable to hold BER value
 * @return error_t 
 */
error_t gsm_modem_get_mt_signal_quality(gsm_modem_t* p_gsm_modem, 
                                                    uint8_t* p_rssi, uint8_t* p_ber);

/**
 * @brief Initialization function that should be called before any other functions
 *        in this file. This function handles the initialization of DTE ATModem
 *        layer.
 * 
 * @param p_gsm_modem Pointer to \p gsm_modem_t variable. This variable should be valid
 *                    until gsm_modem_deinit is called. 
 * @param p_dte_config 
 * @return error_t 
 */
error_t gsm_modem_init(gsm_modem_t* p_gsm_modem, const atmodem_config_t* p_dte_config);


/**
 * @brief Deinitialization function that should be called once GSM modem layetr
 *        is no longer needed to free resources. This function deinitializes the
 *        underlying DTE ATModem layer as well.
 * 
 * @param p_gsm_modem Pointer to gsm_modem_t var
 * @return error_t 
 */
error_t gsm_modem_deinit(gsm_modem_t* p_gsm_modem);


#endif