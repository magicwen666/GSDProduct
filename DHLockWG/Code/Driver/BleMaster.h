#ifndef __BLE_MASTER_H__
#define __BLE_MASTER_H__

typedef enum {
    BLE_RET_SUCCESS,
    BLE_RET_FAILURE,
    BLE_RET_ARGERR,
    BLE_RET_TIMEOUT,
} ble_ret_t;


ble_ret_t BleMasterInit(void);
ble_ret_t BleMasterSend(void *data, int len);
ble_ret_t BleMasterRecv(void *data, unsigned int *len);


#endif



