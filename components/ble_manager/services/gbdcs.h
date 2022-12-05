#ifndef ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_GBDCS_H_
#define ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_GBDCS_H_

/**
 * @brief getBrain Data Collecting Service (GBDCS)
 * @defgroup bt_gbdcs getBrain Data Collecting Service (GBDCS)
 */

#ifdef __cplusplus
extern "C" {
#endif

// #define GBDCS_BT_SAMPLE_PER_PACKET 1
#define GBDCS_BT_PACKET_EEG_LEN   8*3
#define GBDCS_BT_PACKET_TOTAL_LEN (GBDCS_BT_PACKET_EEG_LEN+2) //*GBDCS_BT_SAMPLE_PER_PACKET

/** @def BT_UUID_GBDCS_VAL
 *  @brief GBDCS Service UUID value
 */
#define BT_UUID_GBDCS_VAL 0x310d

/** @def BT_UUID_GBDCS
 *  @brief GBDCS Service
 */
#define BT_UUID_GBDCS \
	BT_UUID_DECLARE_16(BT_UUID_GBDCS_VAL)

/** @def BT_UUID_GBDCS_CONTROL_COMMAND_VAL
 *  @brief GBDCS Characteristic Control Command UUID value
 */
#define BT_UUID_GBDCS_CONTROL_COMMAND_VAL 0x4a31

/** @def BT_UUID_GBDCS_CONTROL_COMMAND
 *  @brief GBDCS Characteristic Control Command
 */
#define BT_UUID_GBDCS_CONTROL_COMMAND \
	BT_UUID_DECLARE_16(BT_UUID_GBDCS_CONTROL_COMMAND_VAL)

/** @def BT_UUID_GBDCS_EEG_MEASUREMENT_VAL
 *  @brief GBDCS Characteristic EEG Measurement Interval UUID value
 */
#define BT_UUID_GBDCS_EEG_MEASUREMENT_VAL 0x4a37

/** @def BT_UUID_GBDCS_MEASUREMENT
 *  @brief GBDCS Characteristic EEG Measurement Interval
 */
#define BT_UUID_GBDCS_EEG_MEASUREMENT \
	BT_UUID_DECLARE_16(BT_UUID_GBDCS_EEG_MEASUREMENT_VAL)

/** @brief Callback type for when an command is received. */
typedef void (*cmd_cb_t)(const uint8_t command);

struct bt_gbdcs_cb {
	/** Command change callback. */
	cmd_cb_t    cmd_cb;
};

typedef union
{
    struct
    {
        uint16_t nb_sample;
        uint8_t eeg_data[GBDCS_BT_PACKET_EEG_LEN];
    } frame;
    uint8_t buffer[GBDCS_BT_PACKET_TOTAL_LEN];
} gbdcs_eeg_packet_t;

/** @brief Initialize the LBS Service.
 *
 *
 * @param[in]
 *
 *
 * @retval 
 */
int bt_gbdcs_init(struct bt_gbdcs_cb *callbacks);

/** @brief Notify getBrain eeg data measurement.
 *
 * This will send a GATT notification to all current subscribers.
 *
 *  @param pgb_eegdata The getBrain eeg data package.
 *
 *  @return Zero in case of success and error code in case of error.
 */
int bt_gbdcs_eeg_notify(gbdcs_eeg_packet_t *pgb_eegdata);

#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_BLUETOOTH_SERVICES_GBDCS_H_ */
