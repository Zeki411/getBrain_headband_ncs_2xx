#include <zephyr/types.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <zephyr.h>
#include <init.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/conn.h>
#include <bluetooth/uuid.h>
#include <bluetooth/gatt.h>

#include "gbdcs.h"

#define LOG_LEVEL 4//CONFIG_BT_GBDCS_LOG_LEVEL
#include <logging/log.h>
LOG_MODULE_REGISTER(gbdcs, LOG_LEVEL);

#define GATT_PERM_READ_MASK     (BT_GATT_PERM_READ | \
				 BT_GATT_PERM_READ_ENCRYPT | \
				 BT_GATT_PERM_READ_AUTHEN)
#define GATT_PERM_WRITE_MASK    (BT_GATT_PERM_WRITE | \
				 BT_GATT_PERM_WRITE_ENCRYPT | \
				 BT_GATT_PERM_WRITE_AUTHEN)

#ifndef CONFIG_BT_GBDCS_DEFAULT_PERM_RW_AUTHEN
#define CONFIG_BT_GBDCS_DEFAULT_PERM_RW_AUTHEN 0
#endif
#ifndef CONFIG_BT_GBDCS_DEFAULT_PERM_RW_ENCRYPT
#define CONFIG_BT_GBDCS_DEFAULT_PERM_RW_ENCRYPT 0
#endif
#ifndef CONFIG_BT_GBDCS_DEFAULT_PERM_RW
#define CONFIG_BT_GBDCS_DEFAULT_PERM_RW 0
#endif

#define GBDCS_GATT_PERM_DEFAULT (						\
	CONFIG_BT_GBDCS_DEFAULT_PERM_RW_AUTHEN ?				\
	(BT_GATT_PERM_READ_AUTHEN | BT_GATT_PERM_WRITE_AUTHEN) :	\
	CONFIG_BT_GBDCS_DEFAULT_PERM_RW_ENCRYPT ?				\
	(BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT) :	\
	(BT_GATT_PERM_READ | BT_GATT_PERM_WRITE))			\

static struct bt_gbdcs_cb       gbdcs_cb;

static void gbdcmc_ccc_cfg_changed(const struct bt_gatt_attr *attr, uint16_t value)
{
	ARG_UNUSED(attr);

	bool notif_enabled = (value == BT_GATT_CCC_NOTIFY);

	LOG_INF("GBDCS notifications %s", notif_enabled ? "enabled" : "disabled");
}

static ssize_t write_gbdcs_command(struct bt_conn *conn,
			 const struct bt_gatt_attr *attr,
			 const void *buf,
			 uint16_t len, uint16_t offset, uint8_t flags)
{
	LOG_DBG("Attribute write, handle: %u, conn: %p", attr->handle, (void *)conn);

	if (len != 1U) {
		LOG_DBG("Write command: Incorrect data length");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_ATTRIBUTE_LEN);
	}

	if (offset != 0) {
		LOG_DBG("Write command: Incorrect data offset");
		return BT_GATT_ERR(BT_ATT_ERR_INVALID_OFFSET);
	}

	if (gbdcs_cb.cmd_cb) {
		uint8_t val = *((uint8_t *)buf);
		gbdcs_cb.cmd_cb(val);
		LOG_DBG("Write comnmand: %d", val);
	}

	return len;
}

/* getBrain Data Collecting Service Declaration */
BT_GATT_SERVICE_DEFINE(gbdcs_svc,
	BT_GATT_PRIMARY_SERVICE(BT_UUID_GBDCS),
	BT_GATT_CHARACTERISTIC(BT_UUID_GBDCS_EEG_MEASUREMENT, BT_GATT_CHRC_NOTIFY,
			            	BT_GATT_PERM_NONE, 
							NULL, NULL, NULL),
	BT_GATT_CCC(gbdcmc_ccc_cfg_changed, GBDCS_GATT_PERM_DEFAULT),
	BT_GATT_CHARACTERISTIC(BT_UUID_GBDCS_CONTROL_COMMAND, BT_GATT_CHRC_WRITE,
			    			BT_GATT_PERM_WRITE, 
							NULL, write_gbdcs_command, NULL),

);

int bt_gbdcs_init(struct bt_gbdcs_cb *callbacks)
{
	if (callbacks) {
		gbdcs_cb.cmd_cb = callbacks->cmd_cb;
	}

	return 0;
}

int bt_gbdcs_eeg_notify(gbdcs_eeg_packet_t *pgb_eegdata)
{
	int rc;
	static uint8_t gbdcs_data[GBDCS_BT_PACKET_TOTAL_LEN];

    for(uint8_t u8cnt = 0; u8cnt < GBDCS_BT_PACKET_TOTAL_LEN; u8cnt++)
    {
        gbdcs_data[u8cnt] = pgb_eegdata->buffer[u8cnt];
    }

	rc = bt_gatt_notify(NULL, &gbdcs_svc.attrs[1], gbdcs_data, GBDCS_BT_PACKET_TOTAL_LEN);

	return rc == -ENOTCONN ? 0 : rc;
}