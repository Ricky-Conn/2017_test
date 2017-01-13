/***********************************************************************************
 * 文 件 名   : transfer_profile.c
 * 创 建 者   : liuyuanbin
 * 创建日期   : 2017年1月4日
 * 版权说明   : Copyright (c) 2016-2025   广东乐心医疗电子股份有限公司
 * 文件描述   : 进行通信协议层的编写，使用与驱动无关的通信，进行简单的接收数据组包和
 				发送数据组包的功能，调用驱动的接口进行收发
 * 修改历史   : 
***********************************************************************************/

/*******************   头文件  ***************************/
#include "transfer_profile.h"
#include "transfer_driver.h"
#include "transfer_task.h"
#include "freertos_fit.h"
#include <string.h>
#include "crc_32.h"
#include "mode_config.h"
#include "debug.h"


/*******************  全局变量 ***************************/
transfer_profile_st g_transfer_profile_st;
uint8_t g_receive_finish_flg = 0;
static uint16_t g_profile_pack_no;
static uint8_t g_transmit_queue_insert = 0;
static uint8_t g_transmit_queue_tx = 0;
static transmit_queue_st g_transmit_queue[TRANSMIT_QUEUE_SIZE];


/*****************************************************************************
 * 函 数 名 : transfer_profile_init
 * 函数功能 : 
    传输协议的初始化
 * 输入参数 : 无
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
*****************************************************************************/
void transfer_profile_init(void)
{
	g_profile_pack_no = TRANSFER_PROFILE_PACK_NO_START;
	
	g_transmit_queue_insert = 0;
	g_transmit_queue_tx = 0;
	memset(g_transmit_queue,0,TRANSMIT_QUEUE_SIZE*sizeof(transmit_queue_st));
}


/*****************************************************************************
 * 函 数 名 : transfer_profile_get_packet_no
 * 函数功能 : 
    传输协议的初始化
 * 输入参数 : 无
 * 输出参数 : 
 	no:输出no的数组形式
 * 返 回 值 : 
 	uint16_t:输出no的值
 * 修改历史 : 无
 * 说    明 : 获取发送数据包的序号，用于通信协议里面的包序号，可以用来区分发送
 				数据分方向
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
 * 函 数 名 : transfer_profile_receive_parse
 * 函数功能 : 
	从驱动层中断里面接收数据并根据长度进行组包
 * 输入参数 : 
 	data:数据指针
 	length:数据长度
 * 输出参数 :无
 * 返 回 值 : 
 	0:接收数据并解析
 	1:接收数据长度 大于 正常规定值
 * 修改历史 : 无
 * 说    明 : 无
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

	/*在本地计算发送过来包的数据 CRC*/
	local_crc = crc32(data, (data_length+PACK_HEAD_SIZE));
	packet_crc = transfer_profile_com_get_value(l_transfer_profile_st.crc,4);
	
	cmd = l_transfer_profile_st.pack_cmd;
	//这里存在1包数据是否有多帧的组包问题
	total = (l_transfer_profile_st.frame_no>>4)&0x0F;
	current = (l_transfer_profile_st.frame_no)&0x0F;

	if(local_crc == packet_crc)
	{
		//校验CRC成功
		//如果是单单1帧数据
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

				/*在多帧组合1包的时候，加起来的长度超过1包最多长度，做长度出错限制*/
				if((pack_length+current_pack_len) > PACK_DATA_SIZE)
					current_pack_len = PACK_DATA_SIZE - pack_length;
				
				memcpy(&(g_transfer_profile_st.data[pack_length]),l_transfer_profile_st.data,current_pack_len);
				pack_length += current_pack_len;
				transfer_profile_com_set_value((uint32_t)(pack_length),g_transfer_profile_st.pack_length,2);
			}
			
			xSemaphoreGiveFromISR(xSemaphore_receive_data,&xHigherPriorityTaskWoken);
	    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);

			if(total == current)//多帧接收完成
			{
				receive_complete_flg = 1;
			}
			else
			{
				/*在多帧接收过程中，每一帧需要给对方回复*/
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
		//校验CRC出错
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
 * 函 数 名 : transfer_profile_receive_loop
 * 函数功能 : 
	从接收层里面接收完数据，并进行解析命令和相应的操作
 * 输入参数 :无
 * 输出参数 :无
 * 返 回 值 : 
 	0:接收数据并解析
 * 修改历史 : 无
 * 说    明 : 这个函数在transfer_task里面调用，当有数据接收完成进行解析和操作
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
 * 函 数 名 : transfer_profile_transmit_data
 * 函数功能 : 
	把需要发送的数据以队列的形式添加进发送队列
 * 输入参数 :
 	data:数据指针
 	length:数据长度
 * 输出参数 :无
 * 返 回 值 : 
 	0:需要发送的数据成功添加到发送buffer里面去了
 * 修改历史 : 无
 * 说    明 : 当应用层有数据要发送的时候，就可以调用这个函数进行发送数据
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
 * 函 数 名 : transfer_profile_transmit_loop
 * 函数功能 : 
	循环查找需要发送的数据队列，有的话就发送
 * 输入参数 :
 	data:数据指针
 	length:数据长度
 * 输出参数 :无
 * 返 回 值 : 
 	0:需要发送的数据成功添加到发送buffer里面去了
 * 修改历史 : 无
 * 说    明 : 当发送队列里面有数据需要发送的时候，就会在transfer_driver_task
 				循环调用这个函数
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
		//数据已经发送完成
		return 0;
	}
	else
	{
		if (!err_code)
		{
			//数据发送成功，但还有其他数据没有发送完
			return 1;
		}
		else
		{
			//数据因阻塞或者其他原因没有发送成功
			return 2;
		}
	}
}


/*****************************************************************************
 * 函 数 名 : transfer_profile_add_crc_transmit
 * 函数功能 : 
	增加CRC后发送数据
 * 输入参数 :
 	l_transfer_profile_st:发送数据的结构体
 * 输出参数 :无
 * 返 回 值 : 
 	0:计算并添加CRC成功，并发送
 * 修改历史 : 无
 * 说    明 : 一般需要发送数据时，都是先填充发送数据的结构体相应的数据，填完以
 				后再进行计算CRC,并填充到当前结构体中，然后发送
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


