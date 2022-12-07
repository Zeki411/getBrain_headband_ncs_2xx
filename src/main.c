/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>

#include "../components/ble_manager/BLEManager.h"
#include "../components/drivers/ads1299/ads1299.h"

#define STACKSIZE 1024
#define THREAD0_PRIORITY 6
#define MAX_QUEUE_LENGTH 50
/* Init data queue */
struct k_msgq gb_ads1299_msgq;
K_MSGQ_DEFINE(gb_ads1299_msgq, (ADS1299_NUMBER_OF_CHANNEL * ADS1299_BYTEDATA_RESOLUTION), MAX_QUEUE_LENGTH, 4);

gbdcs_eeg_packet_t GBDC_EEG_Packet;

static void app_gbdcs_cmd_handle(uint8_t cmd) 
{
    if(cmd == 0x01)
    {
        // Start gbdcs notify
        ADS1299_CmdSTART();
    }

    if(cmd == 0x02)
    {
        // Stop gbdcs notify
        ADS1299_CmdSTOP();
        GBDC_EEG_Packet.frame.nb_sample = 0;
    }
}

static struct bt_gbdcs_cb gbdcs_cb = {
    .cmd_cb = app_gbdcs_cmd_handle,
};

void app_ads1299_data_done(void)
{
    while (k_msgq_put(&gb_ads1299_msgq, &ADS1299_EEGRawDataBuffer[0][3], K_NO_WAIT) != 0) {
        /* message queue is full: purge old data & try again */
        k_msgq_purge(&gb_ads1299_msgq);
    }
}

void main(void)
{
	ADS1299_Platform_Init();
    ADS1299_Platform_DataDoneCB_Init(app_ads1299_data_done);
	ADS1299_Init();

    BLEManager_Init();
    BLEManager_GBDCS_Init(&gbdcs_cb);

    while (1)
    {
        if(gb_ads1299_msgq.used_msgs != 0){
            k_msgq_get(&gb_ads1299_msgq, &GBDC_EEG_Packet.frame.eeg_data[0], K_FOREVER);
            BLEManager_GBDCS_EEG_Notify((uint8_t *)&GBDC_EEG_Packet.buffer[0]);
            GBDC_EEG_Packet.frame.nb_sample++;
        }  
    }
}

// K_THREAD_DEFINE(thread0_id, STACKSIZE, thread0, NULL, NULL, NULL,
// 		THREAD0_PRIORITY, 0, 0);

// SYS_INIT(ADS1299_Platform_Init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);