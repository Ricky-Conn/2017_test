#ifndef _BLE_CONFIG_H_
#define _BLE_CONFIG_H_
#include <stdint.h>
#include "mode_config.h"

#define BLE_ADV_NAME_SIZE	(32)
#define DEVICE_ID_SIZE		(12)
#define DEVICE_TYPE_SIZE	(15)


typedef enum
{
	MODE_NO = 0x00,
	MODE_CONFIG,
	MODE_WORK,
	MODE_MAX
}system_mode_em;

typedef enum
{
	STATUS_NO = 0x00,
	STATUS_ADV,
	STATUS_CONN,
	STATUS_MAX
}system_status_em;

typedef struct
{
	uint8_t adv_switch;	//广播开关
	uint8_t adv_type;	//广播类型
	uint16_t adv_interval;//广播间隔
}adv_parm_st;

typedef struct
{
	uint8_t con_mode;		//连接模式
	uint16_t min_interval;	//连接最新间隔
	uint16_t max_interval;	//连接最间隔
	uint16_t latency;		//
	uint16_t timeOut;		//
}con_parm_st;

typedef struct
{
	uint8_t mac_type;		//mac地址类型
	uint8_t mac_addr[6];	//mac地址
	uint8_t ble_name[BLE_ADV_NAME_SIZE];//广播名字
}ble_parm_st;

typedef struct
{
	uint8_t  id_hex[6];
	uint8_t  type[DEVICE_TYPE_SIZE];
	uint8_t  id[DEVICE_ID_SIZE];
}device_parm_st;

typedef struct
{
	uint8_t notify_switch;		//通知总开关
	uint8_t notify_flag[2];		//通知细分开关
	uint8_t ancs_content;		//当使用IOS手机时，ANCS获取内容选择
	uint8_t ancs_content_length[16];//当使用IOS手机时，获取相应内容的长度
}notify_parm_st;

typedef struct
{
    const char fw_ver[5];		//固件版本
    const char hw_ver[5];		//硬件版
	const char all_ver[20];		//all版本
    const char product_type[6];	//产品类型
	const char manufact_name[16];//厂商名字
	const char MCU_type[6];		//MCU类型
	const char SDK_ver[6];		//SDK版本
}version_parm_st;

typedef struct
{
	uint8_t mode;	//模式
	uint8_t status;	//状态
	adv_parm_st adv_parm;
	con_parm_st con_parm;
	ble_parm_st ble_parm;
	device_parm_st device_parm;
	notify_parm_st notify_parm;
	version_parm_st version_parm;

	uint32_t config_init_flag;
	uint8_t config_operation[CONFIG_CMD_MAX];
}mode_config_st;


extern mode_config_st g_mode_config;

void ble_config_param_init(void);

#endif

