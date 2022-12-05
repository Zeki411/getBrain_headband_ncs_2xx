#ifndef __BLE_MANAGER__
#define __BLE_MANAGER__

#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <soc.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "./services/gbdcs.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "../common/Std_Types.h"
#include "./services/gbdcs.h"

extern void BLEManager_GBDCS_EEG_Notify(gbdcs_eeg_packet_t *getBrain_data_packet);
extern void BLEManager_Init(void);
extern void BLEManager_GBDCS_Init(struct bt_gbdcs_cb *callbacks);

#ifdef __cplusplus
}
#endif

#endif