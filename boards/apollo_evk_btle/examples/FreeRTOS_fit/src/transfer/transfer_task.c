/***********************************************************************************
 * 文 件 名   : transfer_task.c
 * 创 建 者   : liuyuanbin
 * 创建日期   : 2017年1月4日
 * 版权说明   : Copyright (c) 2016-2025   广东乐心医疗电子股份有限公司
 * 文件描述   : 进行通信协议任务的开发，这里只负责根据通信文档协议的通信，不需要理会
 				协议的组包等问题，更不用管传输的驱动，做纯任务之间的通信流程
 * 修改历史   : 
***********************************************************************************/

/*******************   头文件  ***************************/
#include "transfer_task.h"
#include "transfer_profile.h"
#include "transfer_driver.h"
#include "mode_config.h"
#include "debug.h"
#include "ble_config.h"


/*******************  全局变量 ***************************/
/* 软定时器handl */
#define HANDLE_50MS_TIMER 1
TaskHandle_t x_transfer_task;
xSemaphoreHandle xSemaphore_transfer;
/* 软定时器的参数 */
TimerHandle_t xTimers_50ms;
uint8_t g_task_transmit_flg = 0;


/*****************************************************************************
 * 函 数 名 : transfer_task_tx_timer_callback
 * 函数功能 : 
    注册一个发送传输协议定时器的回调函数，当有需要进行，调用这个定时回调
    定时去发送用户数据
 * 输入参数 : 
 	xTimer:无使用
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
*****************************************************************************/
void transfer_task_tx_timer_callback(TimerHandle_t xTimer)
{
	QPRINTF("timer 50 ms\r\n");
	g_task_transmit_flg = 1;
	xSemaphoreGive(xSemaphore_transfer);
}


/*****************************************************************************
 * 函 数 名 : transfer_task
 * 函数功能 : 
    传输协议的任务
 * 输入参数 : 
 	pvParameters:没使用
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 进行协议之间的任务的通信，比如在这个任务里面实现配置，OTA等。也
 				可以根据 其他任务的需要进行传输相应的数据给BLE这层。
*****************************************************************************/
void transfer_task(void *pvParameters)
{
	uint8_t status;
    QPRINTF("transfer start...\r\n");

	ble_config_param_init();

	xSemaphore_transfer = xSemaphoreCreateBinary();

	if(xSemaphore_transfer == NULL)
		QPRINTF("xSemaphore transfer creat false\r\n");

	/* 创建一个定时器,周期为50ms */
    xTimers_50ms = xTimerCreate("Timer", 5, pdFALSE, (void *)HANDLE_50MS_TIMER, transfer_task_tx_timer_callback);
	
	while(1)
	{
		
		if(g_receive_finish_flg)
		{
			g_receive_finish_flg = 0;
			transfer_profile_receive_loop();
		}

		status = mode_config_loop_process();

		if(status && g_task_transmit_flg ==1)
		{
			g_task_transmit_flg = 0;
			xTimerStart(xTimers_50ms, 0);
		}

		/* 此处阻塞线程 */
        xSemaphoreTake(xSemaphore_transfer, portMAX_DELAY);
	}
}


