#include <zephyr/kernel.h>

#include "config_event.h"
#include "ble_command_event.h"
#include "../../components/ble_manager/BLEManager.h"

#define MODULE ble_command

static void send_ble_command(uint8_t cmd) 
{
    struct ble_command_event *event = new_ble_command_event();
    event->cmd = cmd;

    APP_EVENT_SUBMIT(event);
}

static struct bt_gbdcs_cb gbdcs_cb = {
    .cmd_cb = (void *) send_ble_command,
};

static void init(void)
{
    BLEManager_GBDCS_Init(&gbdcs_cb);
}

static bool app_event_handler(const struct app_event_header *aeh)
{
    if (is_config_event(aeh)) {
		struct config_event *ce = cast_config_event(aeh);

		init();
		return false;
	}
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, config_event);
