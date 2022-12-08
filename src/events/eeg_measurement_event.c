#include <stdio.h>

#include "eeg_measurement_event.h"


static void log_eeg_measurement_event(const struct app_event_header *aeh)
{
	struct eeg_measurement_event *event = cast_eeg_measurement_event(aeh);

	APP_EVENT_MANAGER_LOG(aeh, "%d,%d,%d,%d,%d,%d,%d,%d", event->eeg_data[0],
                                                          event->eeg_data[1],
                                                          event->eeg_data[2],
                                                          event->eeg_data[3],
                                                          event->eeg_data[4],
                                                          event->eeg_data[5],
                                                          event->eeg_data[6],
                                                          event->eeg_data[7]);
}

static void profile_eeg_measurement_event(struct log_event_buf *buf,
				      const struct app_event_header *aeh)
{
}

APP_EVENT_INFO_DEFINE(eeg_measurement_event,
                        ENCODE(),
                        ENCODE(),
                        profile_eeg_measurement_event);

APP_EVENT_TYPE_DEFINE(eeg_measurement_event,
		  log_eeg_measurement_event,
		  &eeg_measurement_event_info,
		  APP_EVENT_FLAGS_CREATE(APP_EVENT_TYPE_FLAGS_INIT_LOG_ENABLE));
