#ifndef _TRANSFER_PROFILE_H_
#define _TRANSFER_PROFILE_H_
#include <stdint.h>

#define TRANSFER_PROFILE_PACK_NO_START		(0)		//����ŵĿ�ʼ
#define TRANSFER_PROFILE_PACK_NO_END		(1000)	//����ŵĽ���

#define PACK_HEAD_SIZE		(9)//������� + �ΰ���� + type + Reserve + cmd + reserve + ������
#define CRC_SIZE			(4)

#define PACK_DATA_SIZE		(100)

#define TRANSMIT_QUEUE_SIZE			(4)		//���ٸ����Ͷ���
#define TRANSMIT_QUEUE_BUFFER_SIZE	(120)	//֧�ֵķ��Ͷ������ݵĴ�С

#define OPERATION_DRIECTION_BIT_MASK		(0xE0)
#define OPERATION_SHIFT_BIT					(5)
#define DATA_TYPE_BIT_MASK					(0x1F)
#define DATA_TYPE_SHIFT_BIT					(0)

typedef enum
{
	PACK_DATA_TYPE = 0x01,	//���ݰ�
	PACK_EVENT_TYPE,		//�¼���
	PACK_CONFIG_TYPE,		//���ð�
	PACK_OTA_TYPE,			//OTA��
	TYPE_MAX
}transfer_profile_type_em;

typedef enum
{
	READ_DIREC = 0x01,		//������
	WRITE_DIREC,			//д����
	DIREC_MAX
}transfer_profile_cmd_direc_em;

typedef struct{
	uint8_t pack_no[2];	//�������
	uint8_t frame_no;	//�ΰ����	
	uint8_t pack_type;	//������
	uint8_t reserve1;	//�����ֽ�
	uint8_t pack_cmd;	//������
	uint8_t reserve2;	//�����ֽ�
	uint8_t pack_length[2];//������
	uint8_t data[PACK_DATA_SIZE];//����
	uint8_t crc[4];		//CRCУ��
}transfer_profile_st;

/*���Ͷ��нṹ��*/
typedef struct{
	uint8_t flag;	//���Ϳ���
	uint8_t length;	//���ͳ���
	uint8_t buffer[TRANSMIT_QUEUE_BUFFER_SIZE];//�������ݵ�buffer
}transmit_queue_st;

extern uint8_t g_receive_finish_flg;

void transfer_profile_init(void);
uint16_t transfer_profile_get_packet_no(uint8_t *no);
uint8_t transfer_profile_receive_parse(uint8_t *data,uint16_t length);
uint8_t transfer_profile_receive_loop(void);
uint8_t transfer_profile_transmit_data(uint8_t *data,uint16_t length);
uint8_t transfer_profile_transmit_loop(void);
uint8_t transfer_profile_add_crc_transmit(transfer_profile_st *l_transfer_profile_st);

uint32_t transfer_profile_com_get_value(uint8_t *data,uint8_t size);
void transfer_profile_com_set_value(uint32_t value,uint8_t *data,uint8_t size);


#endif

