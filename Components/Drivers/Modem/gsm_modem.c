#include "gsm_modem.h"
#include "ATmodem.h"

#include "types.h"
#include "assert.h"
#include "log.h"
#include "stdio_public.h"

#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>


#define GSM_MODEM_TAG                 "GSM_MODEM"

#ifndef GSM_MODEM_LOG_VERBOSITY_SUPRESS
#define GSM_MODEM_LOGW(msg) \
                                LOGW(GSM_MODEM_TAG, "[LINE] %d: %s", __LINE__, (msg))
#define GSM_MODEM_LOGE(msg) \
                                LOGE(GSM_MODEM_TAG, "[LINE] %d: %s", __LINE__, (msg))
#else
#define GSM_MODEM_LOGW(msg)     LOGW(GSM_MODEM_TAG, "%s", (msg))
#define GSM_MODEM_LOGE(msg)     LOGE(GSM_MODEM_TAG, "%s", (msg))
#endif  /* GSM_MODEM_LOG_VERBOSITY_SUPRESS */



const char* str_chsets[] = 
{
    GSM_MODEM_STR_CHSET_GSM,
    GSM_MODEM_STR_CHSET_HEX,
    GSM_MODEM_STR_CHSET_IRA,
    GSM_MODEM_STR_CHSET_PCCP,
    GSM_MODEM_STR_CHSET_PCDN,
    GSM_MODEM_STR_CHSET_UCS2,
    GSM_MODEM_STR_CHSET_UTF8,
    GSM_MODEM_STR_CHSET_8859_1,
    GSM_MODEM_STR_CHSET_8859C,
};

/**
 * @brief Copy a specific parameter given in DCE responses to \p pdest
 * 
 * @param resp Response returned from DCE. Should be in the format 
 *              +RESP: <param0>, <param1>, ..., <paramn>
 * @param pdest Buffer to copy to the specific parameter refered to by \p param_id
 *              A null byte is always inserted at the end of the copied data
 * @param dest_size Size of \p pdest including null byte.
 * @param param_id Parameter posision in \p resp starting from 0. If the specified 
 *                 parameter is double quoted, the qoutes are removed.
 * @return int Number of characters copied or ,would have been copied given sufficient
 *         \p dest_size , from \p resp to \p pdest not counting the null byte.
 */
static int gsm_modem_get_resp_param(const char* resp, char* pdest, 
                                                uint8_t dest_size, uint8_t param_id)
{
    char* start_ptr, *end_ptr;
    uint8_t curr_id;
    uint8_t param_len;
    uint8_t to_copy;

    ASSERT(resp);
    ASSERT(pdest);
    ASSERT(dest_size > 1);

    start_ptr = strchr(resp, ':');
    if(!start_ptr)
    {
        return FAILED;
    }

    start_ptr++;

    while(*start_ptr == ' ')
    {
        start_ptr++;
    }

    if(*start_ptr == '\0')
    {
        return FAILED;
    }

    curr_id = 0;
    while(curr_id < param_id)
    {
        start_ptr = strchr(start_ptr, ',');

        if(start_ptr)
        {
            start_ptr++;
            curr_id++;
        }
        else
        {
            return FAILED;
        }
    }

    if(*start_ptr == '\0')
    {
        return FAILED;
    }

    end_ptr = strchr(start_ptr, ',');

    if(*start_ptr == '"')
    {
        if(*(++start_ptr) == '\0')
        {
            return FAILED;
        }
    }

    if(end_ptr)
    {
        param_len = (uint8_t) (end_ptr - start_ptr);
    }
    else
    {
        param_len = strlen(start_ptr);
    }

    if(start_ptr[param_len - 1] == '"')
    {
        --param_len;
    }

    if(param_len < dest_size)
    {
        to_copy = param_len;   
    }
    else
    {
        to_copy = dest_size - 1;
        LOGW(GSM_MODEM_TAG, "Only %u copied!", to_copy);
    }

    memcpy(pdest, start_ptr, to_copy);
    pdest[to_copy] = '\0';

    return param_len;
}

#if 0
static error_t gsm_modem_handle_unso_creg(gsm_modem_t* p_gsm_modem, const char* resp)
{
    char param[10];

    if(gsm_modem_get_resp_param(resp, param, sizeof(param), 0) == 1)
    {
        LOGD(GSM_MODEM_TAG, "Network status: %c", param[0]);
        if((p_gsm_modem->cs_network_info.reg_status = strtol(param, NULL, 10)) >=
                                                    GSM_MODEM_REGISTERATION_INVALID)
        {
            GSM_MODEM_LOGE("Invalid network registeration status!");
            return FAILED;
        }
    }
    else
    {
        GSM_MODEM_LOGE("Failed to get network status!");
        return FAILED;
    }

    if(gsm_modem_get_resp_param(resp, param, sizeof(param), 1) == GSM_CELL_LAC_SIZE)
    {
        strncpy(p_gsm_modem->cs_network_info.cell_lac, param, 
                                        sizeof(p_gsm_modem->cs_network_info.cell_lac));
        LOGD(GSM_MODEM_TAG, "Cell LAC: %s", p_gsm_modem->cs_network_info.cell_lac);
    }
    else
    {
        GSM_MODEM_LOGW("Failed to get cell LAC!");
        memset(p_gsm_modem->cs_network_info.cell_lac, 0, 
                                        sizeof(p_gsm_modem->cs_network_info.cell_lac));
    }

    if(gsm_modem_get_resp_param(resp, param, sizeof(param), 2) == GSM_CELL_ID_SIZE)
    {
        strncpy(p_gsm_modem->cs_network_info.cell_id, param, 
                                        sizeof(p_gsm_modem->cs_network_info.cell_id));
        LOGD(GSM_MODEM_TAG, "Cell Id: %s", p_gsm_modem->cs_network_info.cell_id);
    }
    else
    {
        GSM_MODEM_LOGW("Failed to get cell Id!");
        memset(p_gsm_modem->cs_network_info.cell_id, 0, 
                                        sizeof(p_gsm_modem->cs_network_info.cell_id));
    }

    return OK;
}
#endif

static error_t gsm_modem_handle_unsolicited_resp(atmodem_rescode_t rescode,
                                                        const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    uint8_t resp_len;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    resp_len = strlen(resp);
    l_ret = FAILED;

    if(ATMODEM_RESCODE_NOT_RECOGNIZED == rescode)
    {
        if((resp_len >= sizeof(GSM_UNSO_CREG)) &&
                        memcmp(resp, GSM_UNSO_CREG, sizeof(GSM_UNSO_CREG) - 1) == 0)
        {
            #if 0
            l_ret = gsm_modem_handle_unso_creg(p_gsm_modem, resp);
            #endif
        }
    }

    return l_ret;
}

static error_t gsm_modem_handle_default(atmodem_rescode_t rescode, 
                                                        const char* resp, void* args)
{
    error_t l_ret;

    (void) args;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("Default handle response failed!");
            break;
        default:
            GSM_MODEM_LOGE(resp);
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_cgmi(atmodem_rescode_t rescode,
                                                        const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    char* manufacturer;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    manufacturer = (char*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(manufacturer, resp, GSM_DCE_MANUFACTURER_MAX_SIZE);
            manufacturer[GSM_DCE_MANUFACTURER_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            memset(manufacturer, 0, GSM_DCE_MANUFACTURER_MAX_SIZE);
            GSM_MODEM_LOGE("+CGMI response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_cgmm(atmodem_rescode_t rescode, 
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    char* model_name;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    model_name = (char*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(model_name, resp, GSM_DCE_MODEL_NAME_MAX_SIZE);
            model_name[GSM_DCE_MODEL_NAME_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            memset(model_name, 0, GSM_DCE_MODEL_NAME_MAX_SIZE);
            GSM_MODEM_LOGE("+CGMM response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

#if CONFIG_GSM_DCE_EXTRA_ATTRS
static error_t gsm_modem_handle_cgmr(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    char* revision;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    revision = (char*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(revision, resp, GSM_DCE_REVISION_MAX_SIZE);
            revision[GSM_DCE_REVISION_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            memset(revision, 0, GSM_DCE_REVISION_MAX_SIZE);
            GSM_MODEM_LOGE("+CGMR response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}
#endif

static error_t gsm_modem_handle_cgsn(atmodem_rescode_t rescode,
                                                const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    char*   imei;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    imei = (char*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(imei, resp, GSM_DCE_IMEI_SIZE);
            imei[GSM_DCE_IMEI_SIZE] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            memset(imei, 0, GSM_DCE_IMEI_SIZE + 1);
            GSM_MODEM_LOGE("+CGSN response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_cimi(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    char* imsi;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    imsi = (char*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(imsi, resp, GSM_DCE_IMSI_SIZE);
            imsi[GSM_DCE_IMSI_SIZE] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            memset(imsi, 0, GSM_DCE_IMSI_SIZE + 1);
            GSM_MODEM_LOGE("+CIMI response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_cscs(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    char param[8];
    gsm_modem_t* p_gsm_modem;
    gsm_modem_chset_t* chset;
    int param_len;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    chset = (gsm_modem_chset_t*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            param_len = gsm_modem_get_resp_param(resp, param, sizeof(param), 0);

            if((param_len > 0) && (param_len < sizeof(param)))
            {
                LOGD(GSM_MODEM_TAG, "Char set: %s", param);
                if((param_len == sizeof(GSM_MODEM_STR_CHSET_GSM) - 1) &&
                            memcmp(param, GSM_MODEM_STR_CHSET_GSM, param_len) == 0)
                {
                    *chset = GSM_MODEM_CHSET_GSM;
                }
                else if((param_len == sizeof(GSM_MODEM_STR_CHSET_HEX) - 1) &&
                            memcmp(param, GSM_MODEM_STR_CHSET_HEX, param_len) == 0)
                {
                    *chset = GSM_MODEM_CHSET_HEX;
                }
                else if((param_len == sizeof(GSM_MODEM_STR_CHSET_IRA) - 1) &&
                            memcmp(param, GSM_MODEM_STR_CHSET_IRA, param_len) == 0)
                {
                    *chset = GSM_MODEM_CHSET_IRA;
                }
                else if((param_len == sizeof(GSM_MODEM_STR_CHSET_UCS2) - 1) &&
                            memcmp(param, GSM_MODEM_STR_CHSET_UCS2, param_len) == 0)
                {
                    *chset = GSM_MODEM_CHSET_UCS2;
                }
                else if((param_len == sizeof(GSM_MODEM_STR_CHSET_UTF8) - 1) &&
                            memcmp(param, GSM_MODEM_STR_CHSET_UTF8, param_len) == 0)
                {
                    *chset = GSM_MODEM_CHSET_UTF8;
                }
                else
                {
                    GSM_MODEM_LOGE("Unkown char. set!");
                    *chset = GSM_MODEM_CHSET_INVALID;
                    l_ret = FAILED;
                }   
            }
            else
            {
                GSM_MODEM_LOGE("Failed to get char. set!");
                l_ret = FAILED;
            }
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("+CSCS response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}


static error_t gsm_modem_handle_cpas(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    gsm_modem_activ_status_t* p_status;
    char param[2];
    int param_len;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    p_status = (gsm_modem_activ_status_t*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            param_len = gsm_modem_get_resp_param(resp, param, sizeof(param), 0);
            if(param_len == 1)
            {
                *p_status = strtol(param, NULL, 10);
                LOGD(GSM_MODEM_TAG, "Phone status: %u", *p_status);

                if(*p_status >= GSM_MODEM_ACTIV_STATUS_INVALID)
                {
                    GSM_MODEM_LOGE("Invalid phone activity status!");
                    *p_status = GSM_MODEM_ACTIV_STATUS_INVALID;
                    l_ret = FAILED;
                }
            }
            else
            {
                GSM_MODEM_LOGE("Failed to get phone activity status!");
                l_ret = FAILED;
            }
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("+CPAS response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_cfun(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    gsm_modem_func_level_t* p_level;
    char param[2];
    int param_len;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    p_level = (gsm_modem_activ_status_t*) p_gsm_modem->priv_resources;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            param_len = gsm_modem_get_resp_param(resp, param, sizeof(param), 0);
            if(param_len == 1)
            {
                *p_level = strtol(param, NULL, 10);
                LOGD(GSM_MODEM_TAG, "Func level: %u", *p_level);

                if(*p_level >= GSM_MODEM_FUNC_LEVEL_INVALID)
                {
                    GSM_MODEM_LOGE("Invalid MT func level!");
                    *p_level = GSM_MODEM_FUNC_LEVEL_INVALID;
                    l_ret = FAILED;
                }
            }
            else
            {
                GSM_MODEM_LOGE("Failed to get MT func level!");
                l_ret = FAILED;
            }
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("+CFUN response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

static error_t gsm_modem_handle_csq(atmodem_rescode_t rescode,
                                                    const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
    uint8_t* p_rssi;
    uint8_t* p_ber;
    void** p_resources;
    char param[3];
    int param_len;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    p_resources = (void**) p_gsm_modem->priv_resources;
    p_rssi = (uint8_t*) p_resources[0];
    p_ber = (uint8_t*) p_resources[1];
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            if(p_rssi)
            {
                param_len = gsm_modem_get_resp_param(resp, param, sizeof(param), 0);
                if(param_len > 0 && param_len < sizeof(param))
                {
                    *p_rssi = strtol(param, NULL, 10);
                    LOGD(GSM_MODEM_TAG, "RSSI: %u", *p_rssi);
                }
                else
                {
                    GSM_MODEM_LOGE("Invalid RSSI");
                    l_ret = FAILED;
                }
            }

            if(p_ber)
            {
                param_len = gsm_modem_get_resp_param(resp, param, sizeof(param), 1);
                if(param_len > 0 && param_len < sizeof(param))
                {
                    *p_ber = strtol(param, NULL, 10);
                    LOGD(GSM_MODEM_TAG, "BER: %u", *p_ber);
                }
                else
                {
                    GSM_MODEM_LOGE("Invalid BER");
                    l_ret = FAILED;
                }
            }
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("+CSQ response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

#if 0
static error_t gsm_modem_handle_cops(atmodem_rescode_t rescode, 
                                            const char* resp, void* args)
{
    char param[GSM_OP_LONG_MAX_SIZE + 1];
    gsm_modem_t* p_gsm_modem;
    gsm_modem_operator_t* p_operator;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    p_operator = &p_gsm_modem->cs_network_info.operator;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            if(gsm_modem_get_resp_param(resp, param, sizeof(param), 0) == 1)
            {
                LOGD(GSM_MODEM_TAG, "Operator mode: %c", param[0]);
                if((p_operator->mode = strtol(param, NULL, 10)) >= 
                                                    GSM_MODEM_OP_SELECTION_INVALID)
                {
                    p_operator->mode = GSM_MODEM_OP_SELECTION_INVALID;
                    GSM_MODEM_LOGE("+COPS: Invalid selection mode!");
                    return FAILED;
                }
            }
            else
            {
                GSM_MODEM_LOGE("Failed to get network status!");
                return FAILED;
            }

            if(gsm_modem_get_resp_param(resp, param, sizeof(param), 1) == 1)
            {
                LOGD(GSM_MODEM_TAG, "Operator format: %c", param[0]);
                if((p_operator->format = strtol(param, NULL, 10)) >= 
                                                        GSM_MODEM_OP_FORMAT_INVALID)
                {
                    p_operator->format = GSM_MODEM_OP_FORMAT_INVALID;
                    GSM_MODEM_LOGE("+COPS: Invalid format!");
                    return FAILED;
                }
            }
            else
            {
                LOGD(GSM_MODEM_TAG, "No operators available");
                p_operator->status = GSM_MODEM_OP_STATUS_UNKOWN;
                return OK;    
            }

            if(gsm_modem_get_resp_param(resp, param, sizeof(param), 2) > 0)
            {
                LOGD(GSM_MODEM_TAG, "Operator: %s", param);
                p_operator->status = GSM_MODEM_OP_STATUS_CURRENT;

                switch(p_operator->format)
                {
                    case GSM_MODEM_OP_FORMAT_LONG:
                        strncpy(p_operator->operator.operator_long, param, 
                                            sizeof(p_operator->operator.operator_long));
                        break;
                    case GSM_MODEM_OP_FORMAT_SHORT:
                        strncpy(p_operator->operator.operator_short, param, 
                                            sizeof(p_operator->operator.operator_short));
                        break;
                    case GSM_MODEM_OP_FORMAT_NUMERIC:
                        strncpy(p_operator->operator.operator_num, param, 
                                            sizeof(p_operator->operator.operator_num));
                        break;
                    default:
                        break;
                }
            }
            else
            {
                GSM_MODEM_LOGE("Failed to get operator name!");
                return FAILED;
            }
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            GSM_MODEM_LOGE("+COPS response failed!");
            break;
        default:
            break;
    }

    return OK;
}
#endif

error_t gsm_modem_get_dce_manufacturer(gsm_modem_t* p_gsm_modem, char* manufacturer)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(manufacturer);

    cmd_desc.cmd = GSM_CMD_GET_MANUFACTURER;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmi;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) manufacturer;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMI send failed");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_dce_model_name(gsm_modem_t* p_gsm_modem, char* model_name)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(model_name);

    cmd_desc.cmd = GSM_CMD_GET_MODEL_NAME;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmm;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) model_name;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                        != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMM send failed");
        return FAILED;
    }

    return OK;
}

#if CONFIG_GSM_DCE_EXTRA_ATTRS
error_t gsm_modem_get_dce_revision(gsm_modem_t* p_gsm_modem, char* revision)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(revision);

    cmd_desc.cmd = GSM_CMD_GET_REVISION;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmr;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) revision;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMR send failed");
        return FAILED;
    }

    return OK;
}
#endif

error_t gsm_modem_get_dce_imei(gsm_modem_t* p_gsm_modem, char* imei)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(imei);

    cmd_desc.cmd = GSM_CMD_GET_IMEI;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgsn;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) imei;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGSN send failed");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_dce_imsi(gsm_modem_t* p_gsm_modem, char* imsi)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(imsi);

    cmd_desc.cmd = GSM_CMD_GET_IMSI;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cimi;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) imsi;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CIMI send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_dce_character_set(gsm_modem_t* p_gsm_modem, 
                                                        gsm_modem_chset_t* chset)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(chset);

    cmd_desc.cmd = GSM_CMD_GET_CHSET;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cscs;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) chset;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CSCS? send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_set_dce_character_set(gsm_modem_t* p_gsm_modem, 
                                                        gsm_modem_chset_t chset)
{
    char cmd_str[16];
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(chset < GSM_MODEM_CHSET_INVALID);

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;
    cmd_desc.cmd_size = snprintf_(cmd_str, sizeof(cmd_str), 
                                                GSM_CMD_SET_CHSET, str_chsets[chset]);
    if(cmd_desc.cmd_size >= sizeof(cmd_str))
    {
        GSM_MODEM_LOGE("cmd buffer is insufficient!");
        return FAILED;
    }
    cmd_desc.cmd = cmd_str;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        LOGE(GSM_MODEM_TAG, "%s send failed!", cmd_str);
        return FAILED;
    }

    return OK;
}

const char* gsm_modem_dce_chset_to_str(gsm_modem_chset_t chset)
{
    if(chset < GSM_MODEM_CHSET_INVALID)
    {
        return str_chsets[chset];
    }

    return NULL;
}


error_t gsm_modem_get_mt_activ_status(gsm_modem_t* p_gsm_modem, 
                                                gsm_modem_activ_status_t* p_status)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(p_status);

    cmd_desc.cmd = GSM_CMD_GET_ACTIV_STATUS;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cpas;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) p_status;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CPAS send failed!");
        return FAILED;
    }

    return OK;
}


error_t gsm_modem_soft_reset(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_SOFT_RESET;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_CMD_SET_FUNC_LEVEL_TIMEOUT;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CFUN=1,1 send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_set_mt_func_level(gsm_modem_t* p_gsm_modem, 
                                                    gsm_modem_func_level_t level)
{
    char cmd_str[8];
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(level < GSM_MODEM_FUNC_LEVEL_INVALID);

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_CMD_SET_FUNC_LEVEL_TIMEOUT;
    cmd_desc.cmd_size = snprintf_(cmd_str, sizeof(cmd_str), 
                                            GSM_CMD_SET_FUNC_LEVEL, level);
    if(cmd_desc.cmd_size >= sizeof(cmd_str))
    {
        GSM_MODEM_LOGE("cmd buffer is insufficient!");
        return FAILED;
    }
    cmd_desc.cmd = cmd_str;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        LOGE(GSM_MODEM_TAG, "%s send failed!", cmd_str);
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_mt_func_level(gsm_modem_t* p_gsm_modem, 
                                                gsm_modem_activ_status_t* p_level)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(p_level);

    cmd_desc.cmd = GSM_CMD_GET_FUNC_LEVEL;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cfun;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    p_gsm_modem->priv_resources = (void*) p_level;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CFUN? send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_mt_signal_quality(gsm_modem_t* p_gsm_modem, 
                                                    uint8_t* p_rssi, uint8_t* p_ber)
{
    atmodem_cmd_desc_t cmd_desc;
    void* resources[2];

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_SIGNAL_QUALITY;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_csq;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    resources[0] = (void*) p_rssi;
    resources[1] = (void*) p_ber;
    p_gsm_modem->priv_resources = resources;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CSQ send failed!");
        return FAILED;
    }

    return OK;
}

#if 0
error_t gsm_modem_set_network_registeration_result_code
                                        (gsm_modem_t* p_gsm_modem, uint8_t verbosity)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(verbosity <= GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_WITH_LOC);

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;
    cmd_desc.cmd_size = 0;

    switch(verbosity)
    {
        case GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_DISABLED:
            cmd_desc.cmd = GSM_CMD_CREG_DISABLED;
            break;
        case GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_ENABLED:
            cmd_desc.cmd = GSM_CMD_CREG_ENABLED;
            break;
        case GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_WITH_LOC:
            cmd_desc.cmd = GSM_CMD_CREG_ENABLED_WITH_LOC;
            break;
    }

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CREG send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_select_operator_format(gsm_modem_t* p_gsm_modem, 
                                                    gsm_modem_op_format_t format)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);
    ASSERT(format < GSM_MODEM_OP_FORMAT_INVALID);

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;
    cmd_desc.cmd_size = 0;

    switch (format)
    {
        case GSM_MODEM_OP_FORMAT_LONG:
            cmd_desc.cmd = GSM_CMD_OP_FORMAT_LONG;
            break;
        case GSM_MODEM_OP_FORMAT_SHORT:
            cmd_desc.cmd = GSM_CMD_OP_FORMAT_SHORT;
            break;
        case GSM_MODEM_OP_FORMAT_NUMERIC:
            cmd_desc.cmd = GSM_CMD_OP_FORMAT_NUMERIC;
            break;
        default:
            return FAILED;
    }

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+COPS send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_select_operator(gsm_modem_t* p_gsm_modem, 
                            gsm_modem_op_selection_t mode, const char* operator)
{
    char cmd_str[32];
    atmodem_cmd_desc_t cmd_desc;
    uint8_t fmt;

    ASSERT(p_gsm_modem);
    ASSERT(mode != GSM_MOEDM_OP_SELECTION_FORMAT);
    ASSERT(mode < GSM_MODEM_OP_SELECTION_INVALID);

    fmt = p_gsm_modem->cs_network_info.operator.format;

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_DCE_OP_SELECTION_TIMEOUT;

    switch(mode)
    {
        case GSM_MODEM_OP_SELECTION_AUTO:
        case GSM_MODEM_OP_SELECTION_DEREGISTER:
            cmd_desc.cmd_size = snprintf_(cmd_str, sizeof(cmd_str), 
                                                GSM_CMD_OP_SELECTION_NO_OP, mode, fmt);
            if(cmd_desc.cmd_size >= sizeof(cmd_str))
            {
                GSM_MODEM_LOGE("cmd buffer is insufficient!");
                return FAILED;
            }
            break;
        case GSM_MODEM_OP_SELECTION_MANUAL:
        case GSM_MODEM_OP_SELECTION_AUTO_MANUAL:
            if(operator)
            {
                cmd_desc.cmd_size = snprintf_(cmd_str, sizeof(cmd_str),
                                            GSM_CMD_OP_SELECTION, mode, fmt, operator);
                if(cmd_desc.cmd_size >= sizeof(cmd_str))
                {
                    GSM_MODEM_LOGE("cmd buffer is insufficient!");
                    return FAILED;
                }
            }
            else
            {
                GSM_MODEM_LOGE("Operator is NULL");
                return FAILED;
            }
            break;
        default:
            return FAILED;
    }

    cmd_desc.cmd = cmd_str;
    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+COPS send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_operator(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_OP_GET;
    cmd_desc.resp_callback = &gsm_modem_handle_cops;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;
    cmd_desc.cmd_size = 0;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+COPS? send failed!");
        return FAILED;
    }

    return OK;
}
#endif

error_t gsm_modem_sync(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    cmd_desc.cmd = "";
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = NULL;
    cmd_desc.timeout_ms = GSM_CMD_DEFAULT_TIMEOUT;

    if(atmodem_send_command(&p_gsm_modem->dte_layer, &cmd_desc) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("AT send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_init(gsm_modem_t* p_gsm_modem, const atmodem_config_t* p_dte_config)
{
    ASSERT(p_gsm_modem);

    memset(p_gsm_modem, 0, sizeof(gsm_modem_t));

    if(atmodem_init(&p_gsm_modem->dte_layer, p_dte_config) != ATMODEM_RETVAL_SUCCESS)
    {
        return FAILED;
    }

    p_gsm_modem->dte_layer.upper_layer = (void*) p_gsm_modem;
    p_gsm_modem->dte_layer.unsco_callback = &gsm_modem_handle_unsolicited_resp;

    if(gsm_modem_sync(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_deinit(gsm_modem_t* p_gsm_modem)
{
    ASSERT(p_gsm_modem);

    if(atmodem_deinit(p_gsm_modem->dte_layer) != ATMODEM_RETVAL_SUCCESS)
    {
        return FAILED;
    }

    memset(p_gsm_modem, 0, sizeof(gsm_modem_t));

    return OK;
}