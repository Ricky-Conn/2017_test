#ifndef _TRANSFER_PROFILE_H_
#define _TRANSFER_PROFILE_H_
#include <stdint.h>

#define TRANSFER_PROFILE_PACK_NO_START		(0)		//包序号的开始
#define TRANSFER_PROFILE_PACK_NO_END		(1000)	//包序号的结束

#define PACK_HEAD_SIZE		(9)//主包序号 + 次包序号 + type + Reserve + cmd + reserve + 包长度
#define CRC_SIZE			(4)

#define PACK_DATA_SIZE		(100)

#define TRANSMIT_QUEUE_SIZE			(4)		//多少个发送队列
#define TRANSMIT_QUEUE_BUFFER_SIZE	(120)	//支持的发送队列数据的大小

#define OPERATION_DRIECTION_BIT_MASK		(0xE0)
#define OPERATION_SHIFT_BIT					(5)
#define DATA_TYPE_BIT_MASK					(0x1F)
#define DATA_TYPE_SHIFT_BIT					(0)

typedef enum
{
	PACK_DATA_TYPE = 0x01,	//数据包
	PACK_EVENT_TYPE,		//事件包
	PACK_CONFIG_TYPE,		//配置包
	PACK_OTA_TYPE,			//OTA包
	TYPE_MAX
}transfer_profile_type_em;

typedef enum
{
	READ_DIREC = 0x01,		//读方向
	WRITE_DIREC,			//写方向
	DIREC_MAX
}transfer_profile_cmd_direc_em;

typedef struct{
	uint8_t pack_no[2];	//主包序号
	uint8_t frame_no;	//次包序号	
	uint8_t pack_type;	//包类型
	uint8_t reserve1;	//保留字节
	uint8_t pack_cmd;	//包命令
	uint8_t reserve2;	//保留字节
	uint8_t pack_length[2];//包长度
	uint8_t data[PACK_DATA_SIZE];//数据
	uint8_t crc[4];		//CRC校验
}transfer_profile_st;

/*发送队列结构体*/
typedef struct{
	uint8_t flag;	//发送开关
	uint8_t length;	//发送长度
	uint8_t buffer[TRANSMIT_QUEUE_BUFFER_SIZE];//发送数据的buffer
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

