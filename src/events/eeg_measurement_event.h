#ifndef _EEG_MEASUREMENT_EVENT_H_
#define _EEG_MEASUREMENT_EVENT_H_

#include <app_event_manager.h>
#include <app_event_manager_profiler_tracer.h>
#include "../../components/drivers/ads1299/ads1299.h"

#ifdef __cplusplus
extern "C" {
#endif

struct eeg_measurement_event {
	struct app_event_header header;
    uint8_t eeg_data[ADS1299_NUMBER_OF_CHANNEL*ADS1299_BYTEDATA_RESOLUTION];
};

APP_EVENT_TYPE_DECLARE(eeg_measurement_event);

#ifdef __cplusplus
}
#endif

#endif /* _EEG_MEASUREMENT_EVENT_H_ */