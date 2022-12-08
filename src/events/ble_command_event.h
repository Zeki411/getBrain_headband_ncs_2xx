#ifndef _BLE_COMMAND_EVENT_H_
#define _BLE_COMMAND_EVENT_H_

#include <app_event_manager.h>
#include <app_event_manager_profiler_tracer.h>
#include "../../components/drivers/ads1299/ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

struct ble_command_event {
	struct app_event_header header;
    uint8_t cmd;
};

APP_EVENT_TYPE_DECLARE(ble_command_event);

#ifdef __cplusplus
}
#endif

#endif /* _BLE_COMMAND_EVENT_H_ */