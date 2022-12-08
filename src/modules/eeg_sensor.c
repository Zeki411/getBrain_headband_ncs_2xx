#include <zephyr/kernel.h>

#include "config_event.h"
#include "ble_command_event.h"
#include "eeg_measurement_event.h"

#define MODULE eeg_sensor

static void eeg_measure(uint8_t *data_buf, uint8_t size)
{
    struct eeg_measurement_event *event = new_eeg_measurement_event();

    memcpy(&event->eeg_data[0], &data_buf[0], 24);

    APP_EVENT_SUBMIT(event);
}

static void init(void)
{
    ADS1299_Platform_DataDoneCB_Init(eeg_measure);
    ADS1299_Init();
}

static bool app_event_handler(const struct app_event_header *aeh)
{
    if(is_config_event(aeh)) {
		struct config_event *ce = cast_config_event(aeh);

		// value1 = ce->init_value1;
		init();
		return false;
	}

    if(is_ble_command_event)
    {
        struct ble_command_event *bce = cast_ble_command_event(aeh);
        if(bce->cmd == 0x01)
        {
            ADS1299_CmdSTART();
        }

        if(bce->cmd == 0x02)
        {
            ADS1299_CmdSTOP();
        }

        return false;
    }

    /* If event is unhandled, unsubscribe. */
	__ASSERT_NO_MSG(false);

    return false;
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, ble_command_event);
APP_EVENT_SUBSCRIBE(MODULE, config_event);