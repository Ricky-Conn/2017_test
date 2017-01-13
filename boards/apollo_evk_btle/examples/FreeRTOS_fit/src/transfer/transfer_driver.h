#ifndef _TRANSFER_DRIVER_H_
#define _TRANSFER_DRIVER_H_
#include "freertos_fit.h"


#define SPI_TX_PIN 24
#define SPI_RX_PIN 16

#define TX_BUFFER_SIZE	(116)
#define RX_BUFFER_SIZE	(116)

extern TaskHandle_t x_transfer_drviver_task;

extern xSemaphoreHandle xSemaphore_driver_tx_rx;

extern xSemaphoreHandle xSemaphore_receive_data;

/*****************************************************************************
 * 函 数 名 : transfer_driver_tx_data
 * 函数功能 : 
    给应用程序发送数据的接口。
 * 输入参数 : 
 	data:数据指针
 	length:数据长度
 * 输出参数 : 无
 * 返 回 值 : 
  	0:发送成功
 	0xFF:发送失败
 * 修改历史 : 无
 * 说    明 : 发送数据前，先去获取发送总线的状态
*****************************************************************************/
uint8_t transfer_driver_tx_data(uint8_t *data,uint16_t length);


/*****************************************************************************
 * 函 数 名 : transfer_driver_task
 * 函数功能 : 
    注册一个发送数据的任务，进行收发数据进行管理，只涉及到传输驱动这一层
 * 输入参数 : 
 	pvParameters:无使用
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
*****************************************************************************/
void transfer_driver_task(void *pvParameters);

#endif

