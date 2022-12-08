# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(getBrain_headband_ncs_2xx)

# Include application application event headers
# Include application events and configuration headers
target_include_directories(app PRIVATE
  src/events
)

target_sources(app PRIVATE 
        src/main.c
        src/modules/eeg_sensor.c
        src/modules/ble_command.c
        src/modules/ble_eeg_notify.c

        src/events/eeg_measurement_event.c
        src/events/ble_command_event.c
        src/events/config_event.c
        
        components/drivers/ads1299/ads1299_cfg.c
        components/drivers/ads1299/ads1299.c
        components/ble_manager/BLEManager.c
        components/ble_manager/services/gbdcs.c
        
)
