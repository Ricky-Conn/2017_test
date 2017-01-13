/***********************************************************************************
 * �� �� ��   : transfer_profile.c
 * �� �� ��   : liuyuanbin
 * ��������   : 2017��1��4��
 * ��Ȩ˵��   : Copyright (c) 2016-2025   �㶫����ҽ�Ƶ��ӹɷ����޹�˾
 * �ļ�����   : ����ͨ��Э���ı�д��ʹ���������޹ص�ͨ�ţ����м򵥵Ľ������������
 				������������Ĺ��ܣ����������Ľӿڽ����շ�
 * �޸���ʷ   : 
***********************************************************************************/

/*******************   ͷ�ļ�  ***************************/
#include "transfer_profile.h"
#include "transfer_driver.h"
#include "transfer_task.h"
#include "freertos_fit.h"
#include <string.h>
#include "crc_32.h"
#include "mode_config.h"
#include "debug.h"


/*******************  ȫ�ֱ��� ***************************/
transfer_profile_st g_transfer_profile_st;
uint8_t g_receive_finish_flg = 0;
static uint16_t g_profile_pack_no;
static uint8_t g_transmit_queue_insert = 0;
static uint8_t g_transmit_queue_tx = 0;
static transmit_queue_st g_transmit_queue[TRANSMIT_QUEUE_SIZE];


/*****************************************************************************
 * �� �� �� : transfer_profile_init
 * �������� : 
    ����Э��ĳ�ʼ��
 * ������� : ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
void transfer_profile_init(void)
{
	g_profile_pack_no = TRANSFER_PROFILE_PACK_NO_START;
	
	g_transmit_queue_insert = 0;
	g_transmit_queue_tx = 0;
	memset(g_transmit_queue,0,TRANSMIT_QUEUE_SIZE*sizeof(transmit_queue_st));
}


/*****************************************************************************
 * �� �� �� : transfer_profile_get_packet_no
 * �������� : 
    ����Э��ĳ�ʼ��
 * ������� : ��
 * ������� : 
 	no:���no��������ʽ
 * �� �� ֵ : 
 	uint16_t:���no��ֵ
 * �޸���ʷ : ��
 * ˵    �� : ��ȡ�������ݰ�����ţ�����ͨ��Э������İ���ţ������������ַ���
 				���ݷַ���
*****************************************************************************/
uint16_t transfer_profile_get_packet_no(uint8_t *no)
{
	uint16_t profile_pack_no;

	profile_pack_no = g_profile_pack_no++;
	if(g_profile_pack_no > TRANSFER_PROFILE_PACK_NO_END)
		g_profile_pack_no = TRANSFER_PROFILE_PACK_NO_START;
	
	no[0] = (profile_pack_no>>8)&0xFF;
	no[1] = (profile_pack_no>>0)&0xFF;
	return profile_pack_no;
}


/*****************************************************************************
 * �� �� �� : transfer_profile_receive_parse
 * �������� : 
	���������ж�����������ݲ����ݳ��Ƚ������
 * ������� : 
 	data:����ָ��
 	length:���ݳ���
 * ������� :��
 * �� �� ֵ : 
 	0:�������ݲ�����
 	1:�������ݳ��� ���� �����涨ֵ
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
uint8_t transfer_profile_receive_parse(uint8_t *data,uint16_t length)
{
	transfer_profile_st l_transfer_profile_st;
	uint8_t total,current,cmd,data_length;
	uint16_t pack_length=0,current_pack_len;
	uint8_t receive_complete_flg = 0;
	uint32_t packet_crc,local_crc;
	portBASE_TYPE xHigherPriorityTaskWoken;
	
	if(length > sizeof(transfer_profile_st))
		return 1;
	
	memcpy(&l_transfer_profile_st,data,9);
	data_length = transfer_profile_com_get_value(l_transfer_profile_st.pack_length,2);
	memcpy(l_transfer_profile_st.data,data+9,data_length);
	memcpy(&l_transfer_profile_st.crc,data+9+data_length,4);

	/*�ڱ��ؼ��㷢�͹����������� CRC*/
	local_crc = crc32(data, (data_length+PACK_HEAD_SIZE));
	packet_crc = transfer_profile_com_get_value(l_transfer_profile_st.crc,4);
	
	cmd = l_transfer_profile_st.pack_cmd;
	//�������1�������Ƿ��ж�֡���������
	total = (l_transfer_profile_st.frame_no>>4)&0x0F;
	current = (l_transfer_profile_st.frame_no)&0x0F;

	if(local_crc == packet_crc)
	{
		//У��CRC�ɹ�
		//����ǵ���1֡����
		if(total == 0)
		{
			if(total == current)
			{
				xSemaphoreTakeFromISR(xSemaphore_receive_data,&xHigherPriorityTaskWoken);
	    		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
				memcpy(&g_transfer_profile_st,&l_transfer_profile_st,sizeof(transfer_profile_st));
				xSemaphoreGiveFromISR(xSemaphore_receive_data,&xHigherPriorityTaskWoken);
	    		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

				receive_complete_flg = 1;
			}
		}
		else
		{
			xSemaphoreTakeFromISR(xSemaphore_receive_data,&xHigherPriorityTaskWoken);
	    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
			
			if(current == 0)
			{
				memcpy(&g_transfer_profile_st,&l_transfer_profile_st,sizeof(transfer_profile_st));
			}
			else
			{
				pack_length = transfer_profile_com_get_value(g_transfer_profile_st.pack_length,2);
				current_pack_len = transfer_profile_com_get_value(l_transfer_profile_st.pack_length,2);

				/*�ڶ�֡���1����ʱ�򣬼������ĳ��ȳ���1����೤�ȣ������ȳ�������*/
				if((pack_length+current_pack_len) > PACK_DATA_SIZE)
					current_pack_len = PACK_DATA_SIZE - pack_length;
				
				memcpy(&(g_transfer_profile_st.data[pack_length]),l_transfer_profile_st.data,current_pack_len);
				pack_length += current_pack_len;
				transfer_profile_com_set_value((uint32_t)(pack_length),g_transfer_profile_st.pack_length,2);
			}
			
			xSemaphoreGiveFromISR(xSemaphore_receive_data,&xHigherPriorityTaskWoken);
	    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

			if(total == current)//��֡�������
			{
				receive_complete_flg = 1;
			}
			else
			{
				/*�ڶ�֡���չ����У�ÿһ֡��Ҫ���Է��ظ�*/
				l_transfer_profile_st.pack_cmd = 0x00;//Ack
				l_transfer_profile_st.data[0] = cmd;
				l_transfer_profile_st.data[1] = 0x00;

				current_pack_len = 0x02;
				transfer_profile_com_set_value((uint32_t)current_pack_len,l_transfer_profile_st.pack_length,2);
				transfer_profile_add_crc_transmit(&l_transfer_profile_st);
			}
		}

		if(receive_complete_flg)
		{
			g_receive_finish_flg = 1;
			QPRINTF("data receive .....end\r\n");
			xSemaphoreGiveFromISR(xSemaphore_transfer,&xHigherPriorityTaskWoken);
	    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
		}
	}
	else
	{
		//У��CRC����
		QPRINTF("packet crc is error!\r\n");
		l_transfer_profile_st.pack_cmd = 0x00;//Ack
		l_transfer_profile_st.data[0] = cmd;
		l_transfer_profile_st.data[1] = 0x01;//crc is error

		current_pack_len = 0x02;
		transfer_profile_com_set_value((uint32_t)current_pack_len,l_transfer_profile_st.pack_length,2);
		transfer_profile_add_crc_transmit(&l_transfer_profile_st);
	}
	
	
	return 0;
}


/*****************************************************************************
 * �� �� �� : transfer_profile_receive_loop
 * �������� : 
	�ӽ��ղ�������������ݣ������н����������Ӧ�Ĳ���
 * ������� :��
 * ������� :��
 * �� �� ֵ : 
 	0:�������ݲ�����
 * �޸���ʷ : ��
 * ˵    �� : ���������transfer_task������ã��������ݽ�����ɽ��н����Ͳ���
*****************************************************************************/
uint8_t transfer_profile_receive_loop(void)
{
	uint8_t type,result;
	transfer_profile_st l_transfer_profile_st;

	xSemaphoreTake(xSemaphore_receive_data,portMAX_DELAY);
	memcpy(&l_transfer_profile_st,&g_transfer_profile_st,sizeof(transfer_profile_st));
	xSemaphoreGive(xSemaphore_receive_data);
	
	type = (l_transfer_profile_st.pack_type) &0x1F;
	
	switch(type)
	{
		case PACK_DATA_TYPE:
			QPRINTF("Data Type\r\n");
			break;
			
		case PACK_EVENT_TYPE:
			QPRINTF("Event Type\r\n");
			break;
			
		case PACK_CONFIG_TYPE:
			QPRINTF("Config Type\r\n");
			result = mode_config_receive_process(&l_transfer_profile_st);
			break;
			
		case PACK_OTA_TYPE:
			QPRINTF("OTA Type\r\n");
			break;
			
		default:
			QPRINTF("Type is error\r\n");
			result = 1;
			break;
	}

	if(!result)
		transfer_profile_add_crc_transmit(&l_transfer_profile_st);

	return result;
}


/*****************************************************************************
 * �� �� �� : transfer_profile_transmit_data
 * �������� : 
	����Ҫ���͵������Զ��е���ʽ��ӽ����Ͷ���
 * ������� :
 	data:����ָ��
 	length:���ݳ���
 * ������� :��
 * �� �� ֵ : 
 	0:��Ҫ���͵����ݳɹ���ӵ�����buffer����ȥ��
 * �޸���ʷ : ��
 * ˵    �� : ��Ӧ�ò�������Ҫ���͵�ʱ�򣬾Ϳ��Ե�������������з�������
*****************************************************************************/
uint8_t transfer_profile_transmit_data(uint8_t *data,uint16_t length)
{
	transmit_queue_st *insert;
	
	insert              = &g_transmit_queue[g_transmit_queue_insert++];
    g_transmit_queue_insert %= TRANSMIT_QUEUE_SIZE;

	insert->flag = 1;
	if(length > TRANSMIT_QUEUE_BUFFER_SIZE)
		insert->length = TRANSMIT_QUEUE_BUFFER_SIZE;
	else
		insert->length = length;

	for(uint16_t i=0;i<insert->length;i++)
		insert->buffer[i] = data[i];

	xSemaphoreGive(xSemaphore_driver_tx_rx);

	return 0;
}



/*****************************************************************************
 * �� �� �� : transfer_profile_transmit_loop
 * �������� : 
	ѭ��������Ҫ���͵����ݶ��У��еĻ��ͷ���
 * ������� :
 	data:����ָ��
 	length:���ݳ���
 * ������� :��
 * �� �� ֵ : 
 	0:��Ҫ���͵����ݳɹ���ӵ�����buffer����ȥ��
 * �޸���ʷ : ��
 * ˵    �� : �����Ͷ���������������Ҫ���͵�ʱ�򣬾ͻ���transfer_driver_task
 				ѭ�������������
*****************************************************************************/
uint8_t transfer_profile_transmit_loop(void)
{
	uint8_t err_code;

    if (g_transmit_queue_tx != g_transmit_queue_insert)
    {
		if(g_transmit_queue[g_transmit_queue_tx].flag)
		{
			err_code = transfer_driver_tx_data(g_transmit_queue[g_transmit_queue_tx].buffer,g_transmit_queue[g_transmit_queue_tx].length);
		}
    
        if (!err_code)
        {
        	g_transmit_queue[g_transmit_queue_tx].flag = 0;
            ++g_transmit_queue_tx;
            g_transmit_queue_tx %= TRANSMIT_QUEUE_SIZE;
        }
    }

	if(g_transmit_queue_tx == g_transmit_queue_insert)
	{
		//�����Ѿ��������
		return 0;
	}
	else
	{
		if (!err_code)
		{
			//���ݷ��ͳɹ�����������������û�з�����
			return 1;
		}
		else
		{
			//������������������ԭ��û�з��ͳɹ�
			return 2;
		}
	}
}


/*****************************************************************************
 * �� �� �� : transfer_profile_add_crc_transmit
 * �������� : 
	����CRC��������
 * ������� :
 	l_transfer_profile_st:�������ݵĽṹ��
 * ������� :��
 * �� �� ֵ : 
 	0:���㲢���CRC�ɹ���������
 * �޸���ʷ : ��
 * ˵    �� : һ����Ҫ��������ʱ����������䷢�����ݵĽṹ����Ӧ�����ݣ�������
 				���ٽ��м���CRC,����䵽��ǰ�ṹ���У�Ȼ����
*****************************************************************************/
uint8_t transfer_profile_add_crc_transmit(transfer_profile_st *l_transfer_profile_st)
{
	uint8_t i,data_length,buffer[128];

	data_length = transfer_profile_com_get_value(l_transfer_profile_st->pack_length,2);
	for(i=0;i<data_length+PACK_HEAD_SIZE;i++)
		buffer[i] = ((uint8_t*)l_transfer_profile_st)[i];

	transfer_profile_com_set_value(crc32(buffer, (data_length+PACK_HEAD_SIZE)),&buffer[data_length+PACK_HEAD_SIZE],4);

	return transfer_profile_transmit_data(buffer,data_length+PACK_HEAD_SIZE + CRC_SIZE);
}

uint32_t transfer_profile_com_get_value(uint8_t *data,uint8_t size)
{
	uint32_t value;
	uint8_t i;
	
	if(size > 4 || size == 0)
		return 0;

	value = 0;
	for(i=0;i<size;i++)
		value |= (uint32_t)(data[i]<<((size-i-1)*8));		

	return value;
}

void transfer_profile_com_set_value(uint32_t value,uint8_t *data,uint8_t size)
{
	uint8_t i;
	
	if(size > 4 || size == 0)
		return;

	for(i=0;i<size;i++)
	{
		data[i] = 0xFF & (value >> ((size-i-1)*8));		
	}
}


