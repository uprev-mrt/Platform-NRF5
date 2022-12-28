/**
  *@file nrf5_gatt_adapter.h
  *@brief header for 
  *@author jason
  *@date 12/27/22
  */

#pragma once
#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

#ifdef MRT_PLATFORM //Location of interface file depends on if we are using adapter in standalone mode or as part of MrT framework
  #include "Utilities/Interfaces/GattInterface/mrt_gatt_interface.h"
#else
  #include "mrt_gatt_interface.h"
#endif
#include "ble_types.h"


/* Exported types ------------------------------------------------------------*/
typedef struct{
   uint16_t conn_handle;
} nrf5_profile_ctx_t;

typedef struct{
  ble_uuid_t nrf_uuid; //Nrf struct for UUID
}nrf5_svc_ctx_t;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */


/* Overrid functions -------------------------------------------------------- 
 *
 *  These functions are declared in mrt_gatt_interface.h and defined weakly in mrt_gatt_interface.c
 *  They should all be overriden in the gatt adapter
 *

**
 * @brief Updates the characteristic value
 * @param chr ptr to char
 * @param data data to update with
 * @param len length of data in bytes
 * @return mrt_status_t 
 *
mrt_status_t mrt_gatt_update_char_val(mrt_gatt_char_t* chr, uint8_t* data, uint16_t len);

**
 * @brief Sends an indication/notification of the event
 * @param chr ptr to char
 * @param data data to update with
 * @param len length of data in bytes
 * @return mrt_status_t 
 *
mrt_status_t mrt_gatt_notify_char_val(mrt_gatt_char_t* chr, uint8_t* data, uint16_t len);

**
 * @brief Updates the character value from the database/api
 * @param chr ptr to char
 * @return status
 *
mrt_status_t mrt_gatt_get_char_val(mrt_gatt_char_t* chr);

**
 * @brief Registers a service with the ble system 
 * @param svc ptr to service
 * @return mrt_status_t 
 *
mrt_status_t mrt_gatt_register_svc(mrt_gatt_svc_t* svc);

*/


#ifdef __cplusplus
}
#endif



