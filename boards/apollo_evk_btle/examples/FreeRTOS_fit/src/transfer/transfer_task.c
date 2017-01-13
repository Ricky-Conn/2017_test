/***********************************************************************************
 * �� �� ��   : transfer_task.c
 * �� �� ��   : liuyuanbin
 * ��������   : 2017��1��4��
 * ��Ȩ˵��   : Copyright (c) 2016-2025   �㶫����ҽ�Ƶ��ӹɷ����޹�˾
 * �ļ�����   : ����ͨ��Э������Ŀ���������ֻ�������ͨ���ĵ�Э���ͨ�ţ�����Ҫ���
 				Э�����������⣬�����ùܴ������������������֮���ͨ������
 * �޸���ʷ   : 
***********************************************************************************/

/*******************   ͷ�ļ�  ***************************/
#include "transfer_task.h"
#include "transfer_profile.h"
#include "transfer_driver.h"
#include "mode_config.h"
#include "debug.h"
#include "ble_config.h"


/*******************  ȫ�ֱ��� ***************************/
/* ��ʱ��handl */
#define HANDLE_50MS_TIMER 1
TaskHandle_t x_transfer_task;
xSemaphoreHandle xSemaphore_transfer;
/* ��ʱ���Ĳ��� */
TimerHandle_t xTimers_50ms;
uint8_t g_task_transmit_flg = 0;


/*****************************************************************************
 * �� �� �� : transfer_task_tx_timer_callback
 * �������� : 
    ע��һ�����ʹ���Э�鶨ʱ���Ļص�������������Ҫ���У����������ʱ�ص�
    ��ʱȥ�����û�����
 * ������� : 
 	xTimer:��ʹ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
void transfer_task_tx_timer_callback(TimerHandle_t xTimer)
{
	QPRINTF("timer 50 ms\r\n");
	g_task_transmit_flg = 1;
	xSemaphoreGive(xSemaphore_transfer);
}


/*****************************************************************************
 * �� �� �� : transfer_task
 * �������� : 
    ����Э�������
 * ������� : 
 	pvParameters:ûʹ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ����Э��֮��������ͨ�ţ������������������ʵ�����ã�OTA�ȡ�Ҳ
 				���Ը��� �����������Ҫ���д�����Ӧ�����ݸ�BLE��㡣
*****************************************************************************/
void transfer_task(void *pvParameters)
{
	uint8_t status;
    QPRINTF("transfer start...\r\n");

	ble_config_param_init();

	xSemaphore_transfer = xSemaphoreCreateBinary();

	if(xSemaphore_transfer == NULL)
		QPRINTF("xSemaphore transfer creat false\r\n");

	/* ����һ����ʱ��,����Ϊ50ms */
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

		/* �˴������߳� */
        xSemaphoreTake(xSemaphore_transfer, portMAX_DELAY);
	}
}


