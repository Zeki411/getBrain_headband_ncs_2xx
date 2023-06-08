/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/zephyr.h>
#include <zephyr/logging/log.h>
#include <app_event_manager.h>

#include "config_event.h"
#include "../components/ble_manager/BLEManager.h"
#include "../components/drivers/ads1299/ads1299.h"

#define MODULE main

LOG_MODULE_REGISTER(MODULE);

#define INIT_VALUE 1

void main(void)
{
    ADS1299_Platform_Init();
    BLEManager_Init();

	if (app_event_manager_init()) {
		LOG_ERR("Application Event Manager not initialized");
	} else {
		struct config_event *event = new_config_event();

		event->init_value1 = INIT_VALUE;
		APP_EVENT_SUBMIT(event);
	}
}

// uint8_t test_arr[24] = {0};

// void main(void)
// {
//     ADS1299_Platform_Init();
// 	ADS1299_Init();
// 	ADS1299_CmdSTART();

// 	while(1)
// 	{
// 		__NOP();
// 		// for(uint8_t idx = 0; idx < 24; idx++)
// 		// {
// 		// 	test_arr[idx]++;
// 		// }

// 		// if(test_arr[0] == 255)
// 		// {
// 		// 	__NOP();
// 		// }
// 	}
// }