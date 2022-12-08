#include <zephyr/kernel.h>

#include "eeg_measurement_event.h"
#include "ble_command_event.h"
#include "../../components/ble_manager/BLEManager.h"

#define MODULE ble_eeg_notify

gbdcs_eeg_packet_t GBDC_EEG_Packet;

static bool app_event_handler(const struct app_event_header *aeh)
{
    if(is_eeg_measurement_event(aeh))
    {
        struct eeg_measurement_event *eme = cast_eeg_measurement_event(aeh);
        memcpy(&GBDC_EEG_Packet.frame.eeg_data[0], &eme->eeg_data[0], GBDCS_BT_PACKET_EEG_LEN);
        BLEManager_GBDCS_EEG_Notify((uint8_t *)&GBDC_EEG_Packet.buffer[0]);
        GBDC_EEG_Packet.frame.nb_sample++;
    }

    if(is_ble_command_event)
    {
        struct ble_command_event *bce = cast_ble_command_event(aeh);
        if(bce->cmd == 0x02)
        {
            GBDC_EEG_Packet.frame.nb_sample = 0;
        }

        return false;
    }
}

APP_EVENT_LISTENER(MODULE, app_event_handler);
APP_EVENT_SUBSCRIBE(MODULE, eeg_measurement_event);
APP_EVENT_SUBSCRIBE(MODULE, ble_command_event);