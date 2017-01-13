#ifndef _MODE_CONFIG_H_
#define _MODE_CONFIG_H_
#include <stdint.h>
#include "transfer_profile.h"

typedef enum
{
	CONFIG_ACk_CMD = 0x00,
	CONFIG_ADV_CMD,
	CONFIG_CONN_CMD,
	CONFIG_NOTIFY_CMD,
	CONFIG_MAC_CMD,
	CONFIG_NAME_CMD,
	CONFIG_DEVICE_ID_CMD,
	CONFIG_DEVICE_TYPE_CMD,
	CONFIG_APP_VERSION,
	CONFIG_BLE_VERSION,
	CONFIG_BLE_HARD_VERSION_CMD,
	CONFIG_ALL_VERSION_CMD,
	CONFIG_PRODUCTS_TYPE_CMD,
	CONFIG_MANUFACT_NAME_CMD,
	CONFIG_BLE_TYPE_CMD,
	CONFIG_CMD_MAX
}packet_config_cmd_em;

uint8_t mode_config_receive_process(transfer_profile_st *l_pack_st);
uint8_t mode_config_loop_process(void);

#endif

