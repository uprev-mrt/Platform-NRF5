/**
  *@file nrf5_gatt_adapter.c
  *@brief header for 
  *@author jason
  *@date 12/27/22
  */

/* Includes ------------------------------------------------------------------*/
#include "nrf5_gatt_adapter.h"
#include "sdk_common.h"
#include "ble_srv_common.h"


/* Private Variables ---------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/

uint16_t static get_16bit_uuid(mrt_gatt_uuid_t uuid)
{
  if(uuid.len == MRT_UUID_LEN_16)
  {
    return uuid.uuid16;
  }
  else 
  {
    return ((uint16_t*)(uuid.uuid128))[6];
  }
}

mrt_status_t mrt_gatt_update_char_val(mrt_gatt_char_t* chr, uint8_t* data, uint16_t len)
{
    mrt_status_t status;
    ble_gatts_value_t gatts_value;
    nrf5_profile_ctx_t* profile_ctx = (nrf5_profile_ctx_t*)chr->svc->pro->ctx; 

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = len;
    gatts_value.offset  = 0;
    gatts_value.p_value = data;

    // Update database.
    status = sd_ble_gatts_value_set( profile_ctx->conn_handle ,
                                      chr->handles.val_handle,
                                      &gatts_value);

    //If we are connected, and notify is enabled, notify
    if((profile_ctx->conn_handle != BLE_CONN_HANDLE_INVALID) && ( chr->props & MRT_GATT_PROP_NOTIFY))
    {
      status = mrt_gatt_notify_char_val(chr, data,len);
    }

    return status;
}


mrt_status_t mrt_gatt_notify_char_val(mrt_gatt_char_t* chr, uint8_t* data, uint16_t len)
{
    ble_gatts_hvx_params_t params;
    nrf5_profile_ctx_t* profile_ctx = (nrf5_profile_ctx_t*)chr->svc->pro->ctx; 


    memset(&params, 0, sizeof(params));
    params.type   = BLE_GATT_HVX_NOTIFICATION;
    params.handle = chr->handles.val_handle;
    params.p_data = data;
    params.p_len  = &len;

    return sd_ble_gatts_hvx(profile_ctx->conn_handle, &params);
}


mrt_status_t mrt_gatt_get_char_val(mrt_gatt_char_t* chr)
{

    mrt_status_t status;
    ble_gatts_value_t gatts_value;
    nrf5_profile_ctx_t* profile_ctx = (nrf5_profile_ctx_t*)chr->svc->pro->ctx; 

    // Initialize value struct.
    memset(&gatts_value, 0, sizeof(gatts_value));

    gatts_value.len     = chr->size;
    gatts_value.offset  = 0;
    gatts_value.p_value = chr->data.value;

    // Update database.
    status = sd_ble_gatts_value_get( profile_ctx->conn_handle ,
                                      chr->handles.val_handle,
                                      &gatts_value);

    //sd_ble_gatts_value_get will set the len to actual bytes copied
    chr->data.len = gatts_value.len;

    return status;
}


mrt_status_t mrt_gatt_register_svc(mrt_gatt_svc_t* svc)
{
    uint32_t              err_code;
    ble_uuid128_t         base_uuid;
    ble_add_char_params_t add_char_params;

    ble_gatts_char_handles_t char_handles;

    //Create the context for the service.
    nrf5_svc_ctx_t* ctx = (nrf5_svc_ctx_t*) malloc(sizeof(nrf5_svc_ctx_t));
    svc->ctx = ctx;



    // Set up BT SIG defined service
    if(svc->uuid.len == MRT_UUID_LEN_16)
    {
      ctx->nrf_uuid.type = BLE_UUID_TYPE_BLE;
      BLE_UUID_BLE_ASSIGN(ctx->nrf_uuid, svc->uuid.uuid16);
    }
    else 
    {

      //This registers the base of a vendor specific UUID 
      // NRF does this so that we can reference the service by its 16bit uuid

      memcpy(base_uuid.uuid128, svc->uuid.uuid128, 16);
      err_code = sd_ble_uuid_vs_add(&base_uuid, &ctx->nrf_uuid.type);
      VERIFY_SUCCESS(err_code);

      //Set 16 bit UUID in ctx so we can reference later if needed
      ctx->nrf_uuid.uuid = get_16bit_uuid(svc->uuid);
    }


    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ctx->nrf_uuid, &svc->handle);
    VERIFY_SUCCESS(err_code);


    //Add Characteristics 
    for(uint32_t i=0; i < svc->charCount; i++)
    {
      //Get ptr to next characteristic
      mrt_gatt_char_t* chr = svc->chars[i];


      //zero out params
      memset(&add_char_params, 0, sizeof(add_char_params));

      add_char_params.uuid              = get_16bit_uuid(chr->uuid);
      add_char_params.uuid_type         = ctx->nrf_uuid.type;
      add_char_params.init_len          = chr->size;
      add_char_params.max_len           = chr->size;

      if(chr->props | MRT_GATT_PROP_READ)
      {
        add_char_params.char_props.read   = 1;
      }
      if(chr->props | MRT_GATT_PROP_WRITE)
      {
        add_char_params.char_props.write   = 1;
      }
      if(chr->props | MRT_GATT_PROP_WRITE_WITHOUT_RESP)
      {
        add_char_params.char_props.write_wo_resp   = 1;
      }
      if(chr->props | MRT_GATT_PROP_SIGNED_WRITE)
      {
        add_char_params.char_props.auth_signed_wr   = 1;
      }
      if(chr->props | MRT_GATT_PROP_BROADCAST)
      {
        add_char_params.char_props.broadcast   = 1;
      }
      if(chr->props | MRT_GATT_PROP_NOTIFY)
      {
        add_char_params.char_props.notify   = 1;
      }

      
      //TODO look at setting proper security options
      add_char_params.read_access       = SEC_OPEN;
      add_char_params.cccd_write_access = SEC_OPEN;

      err_code = characteristic_add(svc->handle, &add_char_params, &char_handles);
      if (err_code != NRF_SUCCESS)
      {
          return err_code;
      }

      //Copy handles to MRT format 
      chr->handles.cccd_handle = char_handles.cccd_handle;
      chr->handles.char_handle = char_handles.user_desc_handle; 
      chr->handles.val_handle = char_handles.value_handle;

    }

    return MRT_STATUS_OK;
}
