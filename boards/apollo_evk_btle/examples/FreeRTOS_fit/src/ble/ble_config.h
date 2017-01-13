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
	uint8_t adv_switch;	//�㲥����
	uint8_t adv_type;	//�㲥����
	uint16_t adv_interval;//�㲥���
}adv_parm_st;

typedef struct
{
	uint8_t con_mode;		//����ģʽ
	uint16_t min_interval;	//�������¼��
	uint16_t max_interval;	//��������
	uint16_t latency;		//
	uint16_t timeOut;		//
}con_parm_st;

typedef struct
{
	uint8_t mac_type;		//mac��ַ����
	uint8_t mac_addr[6];	//mac��ַ
	uint8_t ble_name[BLE_ADV_NAME_SIZE];//�㲥����
}ble_parm_st;

typedef struct
{
	uint8_t  id_hex[6];
	uint8_t  type[DEVICE_TYPE_SIZE];
	uint8_t  id[DEVICE_ID_SIZE];
}device_parm_st;

typedef struct
{
	uint8_t notify_switch;		//֪ͨ�ܿ���
	uint8_t notify_flag[2];		//֪ͨϸ�ֿ���
	uint8_t ancs_content;		//��ʹ��IOS�ֻ�ʱ��ANCS��ȡ����ѡ��
	uint8_t ancs_content_length[16];//��ʹ��IOS�ֻ�ʱ����ȡ��Ӧ���ݵĳ���
}notify_parm_st;

typedef struct
{
    const char fw_ver[5];		//�̼��汾
    const char hw_ver[5];		//Ӳ����
	const char all_ver[20];		//all�汾
    const char product_type[6];	//��Ʒ����
	const char manufact_name[16];//��������
	const char MCU_type[6];		//MCU����
	const char SDK_ver[6];		//SDK�汾
}version_parm_st;

typedef struct
{
	uint8_t mode;	//ģʽ
	uint8_t status;	//״̬
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

