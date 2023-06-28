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

#define GSM_DCE_DEFAULT_CMD_TIMEOUT     300

#define GSM_CMD_GET_MANUFACTURER        "+CGMI"
#define GSM_CMD_GET_MODEL_NAME          "+CGMM"
#define GSM_CMD_GET_REVISION            "+CGMR"
#define GSM_CMD_GET_IMEI                "+CGSN"
#define GSM_CMD_GET_IMSI                "+CIMI"

#define GSM_CMD_CREG_DISABLED           "+CREG=0"
#define GSM_CMD_CREG_ENABLED            "+CREG=1"
#define GSM_CMD_CREG_ENABLED_WITH_LOC   "+CREG=2"


#define GSM_UNSO_CREG                   "+CREG: "

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

typedef struct
{
    char        cell_lac[GSM_CELL_LAC_SIZE + 1];
    char        cell_id[GSM_CELL_ID_SIZE + 1];
    uint8_t     reg_status;     
} gsm_modem_cs_net_info_t;


typedef struct
{
    char manufacturer[GSM_DCE_MANUFACTURER_MAX_SIZE];
    char model_name[GSM_DCE_MODEL_NAME_MAX_SIZE];
    #if CONFIG_GSM_DCE_EXTRA_ATTRS
    char revision[GSM_DCE_REVISION_MAX_SIZE];
    #endif
    char imei[GSM_DCE_IMEI_SIZE + 1];
    char imsi[GSM_DCE_IMSI_SIZE + 1];
} gsm_dce_attr_t;


typedef struct
{
    gsm_dce_attr_t          dce_attr;
    atmodem_layer_t         dte_layer;
    gsm_modem_cs_net_info_t cs_network_info;
} gsm_modem_t;


/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @return error_t 
 */
error_t gsm_modem_get_dce_manufacturer(gsm_modem_t* p_gsm_modem);

/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @return error_t 
 */
error_t gsm_modem_get_dce_model_name(gsm_modem_t* p_gsm_modem);

/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @return error_t 
 */
error_t gsm_modem_get_dce_revision(gsm_modem_t* p_gsm_modem);

/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @return error_t 
 */
error_t gsm_modem_get_dce_imei(gsm_modem_t* p_gsm_modem);


/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @return error_t 
 */
error_t gsm_modem_get_dce_imsi(gsm_modem_t* p_gsm_modem);


/**
 * @brief 
 * 
 * @param p_gsm_modem 
 * @param p_dte_config 
 * @return error_t 
 */
error_t gsm_modem_init(gsm_modem_t* p_gsm_modem, const atmodem_config_t* p_dte_config);

#endif