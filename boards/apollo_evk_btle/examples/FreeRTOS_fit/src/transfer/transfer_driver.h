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
 * �� �� �� : transfer_driver_tx_data
 * �������� : 
    ��Ӧ�ó��������ݵĽӿڡ�
 * ������� : 
 	data:����ָ��
 	length:���ݳ���
 * ������� : ��
 * �� �� ֵ : 
  	0:���ͳɹ�
 	0xFF:����ʧ��
 * �޸���ʷ : ��
 * ˵    �� : ��������ǰ����ȥ��ȡ�������ߵ�״̬
*****************************************************************************/
uint8_t transfer_driver_tx_data(uint8_t *data,uint16_t length);


/*****************************************************************************
 * �� �� �� : transfer_driver_task
 * �������� : 
    ע��һ���������ݵ����񣬽����շ����ݽ��й���ֻ�漰������������һ��
 * ������� : 
 	pvParameters:��ʹ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
void transfer_driver_task(void *pvParameters);

#endif

