#include <stdio.h>

#include "ble_command_event.h"


static void log_ble_command_event(const struct app_event_header *aeh)
{
	struct ble_command_event *event = cast_ble_command_event(aeh);

	APP_EVENT_MANAGER_LOG(aeh, "cmd: %d", event->cmd);
}

static void profile_ble_command_event(struct log_event_buf *buf,
				      const struct app_event_header *aeh)
{
}

APP_EVENT_INFO_DEFINE(ble_command_event,
                        ENCODE(),
                        ENCODE(),
                        profile_ble_command_event);

APP_EVENT_TYPE_DEFINE(ble_command_event,
		  log_ble_command_event,
		  &ble_command_event_info,
		  APP_EVENT_FLAGS_CREATE(APP_EVENT_TYPE_FLAGS_INIT_LOG_ENABLE));
