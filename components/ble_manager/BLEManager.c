#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <sys/printk.h>
#include <sys/byteorder.h>
#include <zephyr.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "../common/Std_Types.h"
#include "./services/gbdcs.h"
#include "BLEManager.h"

#define LOG_LEVEL 3
#include <logging/log.h>
LOG_MODULE_REGISTER(ble_manager);

#define MIN_CONN_INTERVAL 6
#define MAX_CONN_INTERVAL 6
#define SLAVE_LATENCY 0
#define CONN_SUP_TIMEOUT 1000

static void BLEManager_Connected(struct bt_conn *conn, uint8_t err);
static void BLEManager_Disconnected(struct bt_conn *conn, uint8_t reason);
static void BLEManager_BleParamUpdated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout);
static void BLEManager_AuthCancel(struct bt_conn *conn);

struct bt_conn *BLEManger_Connection;

static const struct bt_data ad[] = {
	BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
	BT_DATA_BYTES(BT_DATA_UUID16_ALL,
		    BT_UUID_16_ENCODE(BT_UUID_GBDCS_VAL),)
};

static struct bt_conn_auth_cb auth_cb_display = {
	.cancel = BLEManager_AuthCancel,
};

static struct bt_conn_cb conn_callbacks = {
	.connected = BLEManager_Connected,
	.disconnected = BLEManager_Disconnected,
	.le_param_updated = BLEManager_BleParamUpdated,
};

static void BLEManager_Connected(struct bt_conn *conn, uint8_t err)
{
    struct bt_conn_info info; 
	uint32_t retval;
	uint8_t aU8Addr[BT_ADDR_LE_STR_LEN];
	const struct bt_le_conn_param bt_req_conn_params = {
		.interval_min = MIN_CONN_INTERVAL,
		.interval_max = MAX_CONN_INTERVAL,
		.latency      = SLAVE_LATENCY,
		.timeout      = CONN_SUP_TIMEOUT
	};

    BLEManger_Connection = conn;

	if (err) {
		LOG_ERR("Connection failed (err 0x%02x)\n", err);
	} 
    else if(bt_conn_get_info(conn, &info))
	{
		LOG_ERR("Could not parse connection info\n");
	}
	else
	{
		bt_addr_le_to_str(bt_conn_get_dst(conn), (char *)aU8Addr, sizeof(aU8Addr));
		
		LOG_INF("Connection parameters updated!	\n\
		Connected to: %s						\n\
		New Connection Interval: %u				\n\
		New Slave Latency: %u					\n\
		New Connection Supervisory Timeout: %u	\n\
		New Connection Transmit PHY: %u	\n"
		, (char *)aU8Addr, info.le.interval, info.le.latency, info.le.timeout, info.le.phy->tx_phy);
	}
	
	retval = bt_conn_le_param_update(conn, &bt_req_conn_params);
	if(retval == 0)
	{
		// Procedure request succeeded. Connection parameters will be negotiated as requested.
		// BLE_CONN_PARAMS_EVT_SUCCEEDED will be notified if parameter negotiation is successful.
		// NRF_LOG_INFO("Procedure request succeeded. Connection parameters will be negotiated as requested");
		LOG_INF("Procedure request succeeded. Connection parameters will be negotiated as requested");
	}
	else
	{
		// Procedure request failed.
		// NRF_LOG_INFO("Procedure request failed: %d",retval);
		LOG_ERR("Procedure request failed: %d",retval);
	}
}


static void BLEManager_BleParamUpdated(struct bt_conn *conn, uint16_t interval, uint16_t latency, uint16_t timeout)
{
	struct bt_conn_info info; 
	uint8_t aU8Addr[BT_ADDR_LE_STR_LEN];
	
	if(bt_conn_get_info(conn, &info))
	{
		LOG_ERR("Could not parse connection info\n");
	}
	else
	{
		bt_addr_le_to_str(bt_conn_get_dst(conn), (char *)aU8Addr, sizeof(aU8Addr));

		LOG_INF("Connection parameters updated!	\n\
		Connected to: %s						\n\
		New Connection Interval: %u				\n\
		New Slave Latency: %u					\n\
		New Connection Supervisory Timeout: %u	\n\
		New Connection Transmit PHY: %u	\n"
		, (char *)aU8Addr, info.le.interval, info.le.latency, info.le.timeout, info.le.phy->tx_phy);
	}
}

static void BLEManager_Disconnected(struct bt_conn *conn, uint8_t reason)
{
	LOG_INF("Disconnected (reason 0x%02x)\n", reason);
}

static void BLEManager_AuthCancel(struct bt_conn *conn)
{
	char addr[BT_ADDR_LE_STR_LEN];

	bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

	LOG_INF("Pairing cancelled: %s\n", addr);
}

void BLEManager_Init(void)
{
	int err;

	err = bt_enable(NULL);
	if (err) {
		LOG_ERR("Bluetooth init failed (err %d)\n", err);
		return;
	}

	LOG_INF("Bluetooth initialized\n");

	err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL, 0);
	if (err) {
		LOG_ERR("Advertising failed to start (err %d)\n", err);
		return;
	}

	LOG_INF("Advertising successfully started\n");

	bt_conn_cb_register(&conn_callbacks);
	bt_conn_auth_cb_register(&auth_cb_display);
}

void BLEManager_GBDCS_Init(struct bt_gbdcs_cb *callbacks)
{
	bt_gbdcs_init(callbacks);
}

void BLEManager_GBDCS_EEG_Notify(gbdcs_eeg_packet_t *getBrain_eeg_data_packet)
{	
	bt_gbdcs_eeg_notify(getBrain_eeg_data_packet);
}
