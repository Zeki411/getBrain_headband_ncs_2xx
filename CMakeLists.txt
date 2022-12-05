# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(getBrain_headband_ncs_2xx)

target_sources(app PRIVATE 
        src/main.c
        components/drivers/ads1299/ads1299_cfg.c
        components/drivers/ads1299/ads1299.c
        components/ble_manager/BLEManager.c
        components/ble_manager/services/gbdcs.c
        # components/ble_manager/services/gbctrls.c
)
