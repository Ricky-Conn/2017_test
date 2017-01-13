#include "ble_config.h"
#include "mode_config.h"
#include <string.h>

#define DEVICE_NAME                     "MT3.0"                                      /**< Name of the device. Will be included in the advertising data. */

#define DEVICE_ID                  		"e40415100020"
#define DEVICE_TYPE				   		"gh_4bc17495d97a"

enum
{
    UNIT_0_625_MS = 625,                                /**< Number of microseconds in 0.625 milliseconds. */
    UNIT_1_25_MS  = 1250,                               /**< Number of microseconds in 1.25 milliseconds. */
    UNIT_10_MS    = 10000                               /**< Number of microseconds in 10 milliseconds. */
};

#define MSEC_TO_UNITS(TIME, RESOLUTION) (((TIME) * 1000) / (RESOLUTION))

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(500, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.5 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(1000, UNIT_1_25_MS)           /**< Maximum acceptable connection interval (1 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory time-out (4 seconds). */

/**@defgroup BLE_GAP_ADV_TYPES GAP Advertising types
 * @{ */
#define BLE_GAP_ADV_TYPE_ADV_IND          0x00   /**< Connectable undirected. */
#define BLE_GAP_ADV_TYPE_ADV_DIRECT_IND   0x01   /**< Connectable directed. */
#define BLE_GAP_ADV_TYPE_ADV_SCAN_IND     0x02   /**< Scannable undirected. */
#define BLE_GAP_ADV_TYPE_ADV_NONCONN_IND  0x03   /**< Non connectable undirected. */

mode_config_st g_mode_config = {
	.version_parm.fw_ver[0] = 't',
	.version_parm.fw_ver[1] = 'e',
	.version_parm.fw_ver[2] = 's',
	.version_parm.fw_ver[3] = 't',
	.version_parm.fw_ver[4] = '\0',

	.version_parm.hw_ver[0] = 't',
	.version_parm.hw_ver[1] = 't',
	.version_parm.hw_ver[2] = 't',
	.version_parm.hw_ver[3] = 'h',
	.version_parm.hw_ver[4] = '\0',

	.version_parm.all_ver[0] = 't',
	.version_parm.all_ver[1] = 't',
	.version_parm.all_ver[2] = 't',
	.version_parm.all_ver[3] = 'a',
	.version_parm.all_ver[4] = '\0',

	.version_parm.product_type[0] = 't',
	.version_parm.product_type[1] = 't',
	.version_parm.product_type[2] = 'p',
	.version_parm.product_type[3] = 't',
	.version_parm.product_type[4] = '\0',

	.version_parm.manufact_name[0] = 'l',
	.version_parm.manufact_name[1] = 'i',
	.version_parm.manufact_name[2] = 'f',
	.version_parm.manufact_name[3] = 'e',
	.version_parm.manufact_name[4] = 's',
	.version_parm.manufact_name[5] = 'e',
	.version_parm.manufact_name[6] = 'n',
	.version_parm.manufact_name[7] = 's',
	.version_parm.manufact_name[8] = 'e',
	.version_parm.manufact_name[9] = '\0',
};

void ble_config_param_init(void)
{
	g_mode_config.mode = MODE_CONFIG;
	g_mode_config.status = 0;
	
	
	g_mode_config.adv_parm.adv_switch 	= 1;
	g_mode_config.adv_parm.adv_type 	= BLE_GAP_ADV_TYPE_ADV_IND;
	g_mode_config.adv_parm.adv_interval = 100;

	g_mode_config.con_parm.con_mode 	= 0x01;
	g_mode_config.con_parm.min_interval = MIN_CONN_INTERVAL;
	g_mode_config.con_parm.max_interval = MAX_CONN_INTERVAL;
	g_mode_config.con_parm.latency		= SLAVE_LATENCY;
	g_mode_config.con_parm.timeOut		= CONN_SUP_TIMEOUT;

	g_mode_config.ble_parm.mac_type		= 0x01;
	memcpy(g_mode_config.ble_parm.ble_name,(const uint8_t *)DEVICE_NAME,strlen(DEVICE_NAME));
	g_mode_config.ble_parm.mac_addr[0] 	= 0xE2;
	g_mode_config.ble_parm.mac_addr[1] 	= 0xDC;
	g_mode_config.ble_parm.mac_addr[2] 	= 0x52;
	g_mode_config.ble_parm.mac_addr[3] 	= 0x00;
	g_mode_config.ble_parm.mac_addr[4] 	= 0x31;
	g_mode_config.ble_parm.mac_addr[5] 	= 0xAB;

	memcpy(g_mode_config.device_parm.id_hex, g_mode_config.ble_parm.mac_addr, 6);
	memcpy(g_mode_config.device_parm.id, DEVICE_ID, 12);
    memcpy(g_mode_config.device_parm.type, DEVICE_TYPE, 15);

	g_mode_config.config_init_flag |= (1<<CONFIG_ADV_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_CONN_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_NOTIFY_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_MAC_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_NAME_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_DEVICE_ID_CMD);
	g_mode_config.config_init_flag |= (1<<CONFIG_DEVICE_TYPE_CMD);

	g_mode_config.config_operation[CONFIG_ADV_CMD] 		= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_CONN_CMD] 	= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_NOTIFY_CMD] 	= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_MAC_CMD] 		= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_NAME_CMD] 	= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_DEVICE_ID_CMD]= WRITE_DIREC;
	g_mode_config.config_operation[CONFIG_DEVICE_TYPE_CMD] = WRITE_DIREC;
}



