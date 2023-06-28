#include "gsm_modem.h"
#include "ATmodem.h"

#include "types.h"
#include "assert.h"
#include "log.h"

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

    return to_copy;
}

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
            l_ret = gsm_modem_handle_unso_creg(p_gsm_modem, resp);
        }
    }

    return l_ret;
}

static error_t gsm_modem_handle_default(atmodem_rescode_t rescode, 
                                                        const char* resp, void* args)
{
    gsm_modem_t* p_gsm_modem;
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
    gsm_dce_attr_t* dce;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    dce = &p_gsm_modem->dce_attr;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(dce->manufacturer, resp, GSM_DCE_MANUFACTURER_MAX_SIZE);
            dce->manufacturer[GSM_DCE_MANUFACTURER_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            strncpy(dce->manufacturer, "ERROR", GSM_DCE_MANUFACTURER_MAX_SIZE);
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
    gsm_dce_attr_t* dce;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    dce = &p_gsm_modem->dce_attr;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(dce->model_name, resp, GSM_DCE_MODEL_NAME_MAX_SIZE);
            dce->model_name[GSM_DCE_MODEL_NAME_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            strncpy(dce->model_name, "ERROR", GSM_DCE_MODEL_NAME_MAX_SIZE);
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
    gsm_dce_attr_t* dce;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    dce = &p_gsm_modem->dce_attr;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(dce->revision, resp, GSM_DCE_REVISION_MAX_SIZE);
            dce->revision[GSM_DCE_REVISION_MAX_SIZE - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            strncpy(dce->revision, "ERROR", GSM_DCE_REVISION_MAX_SIZE);
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
    gsm_dce_attr_t* dce;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    dce = &p_gsm_modem->dce_attr;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(dce->imei, resp, sizeof(dce->imei));
            dce->imei[sizeof(dce->imei) - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            strncpy(dce->imei, "ERROR", sizeof(dce->imei));
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
    gsm_dce_attr_t* dce;
    error_t l_ret;

    ASSERT(resp);
    ASSERT(args);

    p_gsm_modem = (gsm_modem_t*) args;
    dce = &p_gsm_modem->dce_attr;
    l_ret = OK;

    switch(rescode)
    {
        case ATMODEM_RESCODE_NOT_RECOGNIZED:
            strncpy(dce->imsi, resp, sizeof(dce->imsi));
            dce->imsi[sizeof(dce->imsi) - 1] = '\0';
            break;
        case ATMODEM_RESCODE_OK:
            break;
        case ATMODEM_RESCODE_ERROR:
            strncpy(dce->imsi, "ERROR", sizeof(dce->imsi));
            GSM_MODEM_LOGE("+CIMI response failed!");
            break;
        default:
            l_ret = FAILED;
            break;
    }

    return l_ret;
}

error_t gsm_modem_get_dce_manufacturer(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_MANUFACTURER;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmi;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMI send failed");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_dce_model_name(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_MODEL_NAME;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmm;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMM send failed");
        return FAILED;
    }

    return OK;
}

#if CONFIG_GSM_DCE_EXTRA_ATTRS
error_t gsm_modem_get_dce_revision(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_REVISION;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgmr;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGMR send failed");
        return FAILED;
    }

    return OK;
}
#endif

error_t gsm_modem_get_dce_imei(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_IMEI;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cgsn;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CGSN send failed");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_get_dce_imsi(gsm_modem_t* p_gsm_modem)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.cmd = GSM_CMD_GET_IMSI;
    cmd_desc.cmd_size = 0;
    cmd_desc.resp_callback = &gsm_modem_handle_cimi;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CIMI send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_set_network_registeration_result_code
                                        (gsm_modem_t* p_gsm_modem, uint8_t verbosity)
{
    atmodem_cmd_desc_t cmd_desc;

    ASSERT(p_gsm_modem);

    cmd_desc.resp_callback = &gsm_modem_handle_default;
    cmd_desc.timeout_ms = GSM_DCE_DEFAULT_CMD_TIMEOUT;
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
        default:
            return FAILED;
    }

    if(atmodem_send_command_wait(&p_gsm_modem->dte_layer, &cmd_desc, portMAX_DELAY) 
                                                    != ATMODEM_RETVAL_SUCCESS)
    {
        GSM_MODEM_LOGE("+CREG send failed!");
        return FAILED;
    }

    return OK;
}

error_t gsm_modem_init(gsm_modem_t* p_gsm_modem, const atmodem_config_t* p_dte_config)
{
    error_t l_ret;

    ASSERT(p_gsm_modem);

    if(atmodem_init(&p_gsm_modem->dte_layer, p_dte_config) != ATMODEM_RETVAL_SUCCESS)
    {
        return FAILED;
    }

    p_gsm_modem->dte_layer.args = (void*) p_gsm_modem;
    p_gsm_modem->dte_layer.unsco_callback = &gsm_modem_handle_unsolicited_resp;

    if(gsm_modem_get_dce_manufacturer(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    if(gsm_modem_get_dce_model_name(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    if(gsm_modem_get_dce_imei(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    if(gsm_modem_get_dce_imsi(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    if(gsm_modem_get_dce_imsi(p_gsm_modem) != OK)
    {
        return FAILED;
    }

    if(gsm_modem_set_network_registeration_result_code(p_gsm_modem, 
                        GSM_MODEM_NETWORK_REGISTERATION_UNSO_RESCODE_WITH_LOC) != OK)
    {
        return FAILED;
    }

    return OK;
}