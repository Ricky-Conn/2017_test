/***********************************************************************************
 * �� �� ��   : transfer_driver.c
 * �� �� ��   : liuyuanbin
 * ��������   : 2017��1��4��
 * ��Ȩ˵��   : Copyright (c) 2016-2025   �㶫����ҽ�Ƶ��ӹɷ����޹�˾
 * �ļ�����   : ����ͨ��Э��������ı�д��Ŀǰʹ��SPI�������Ժ���Ҫ�޸�ͨ�Žӿڵ�ʱ��
 				ֻ��Ҫ�޸���������ļ���
 * �޸���ʷ   : 
***********************************************************************************/

/*******************   ͷ�ļ�  ***************************/
#include "transfer_driver.h"
#include "transfer_profile.h"


/*******************  ȫ�ֱ��� ***************************/
/* ��ʱ��handl */
#define HANDLE_100MS_TIMER 1


/*******************   �궨��  ***************************/
TaskHandle_t x_transfer_drviver_task;
xSemaphoreHandle xSemaphore_driver_tx_rx;
xSemaphoreHandle xSemaphore_receive_data;
TimerHandle_t xTimers_100ms;
uint8_t g_transmit_flg = 0;


/*****************************************************************************
 * �� �� �� : transfer_driver_bus_status
 * �������� : 
    ��������ǰ����ȥ��ȡ�����������ߵ�״̬
 * ������� : ��
 * ������� : ��
 * �� �� ֵ : 
  	0:æ
 	1:����
 * �޸���ʷ : ��
 * ˵    �� : ������IO�ڣ�����Ϊ�ͣ�æΪ��
*****************************************************************************/
static uint8_t transfer_driver_bus_status(void)
{
    if(am_hal_gpio_input_bit_read(SPI_RX_PIN))
    {
        /* ����Ϊ��,��æ */
        return 0;
    }

    am_hal_gpio_out_bit_set(SPI_TX_PIN);

    if(am_hal_gpio_input_bit_read(SPI_RX_PIN))
    {
        am_hal_gpio_out_bit_clear(SPI_TX_PIN);
        return 0;
    }

    return 1;
}


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
uint8_t transfer_driver_tx_data(uint8_t *data,uint16_t length)
{
    uint8_t ret;
    unsigned short i;
    unsigned char *ptr_spi_buf;
    
    ret = 0xFF;

    if(transfer_driver_bus_status())
    {
        ptr_spi_buf = (unsigned char *)(am_hal_ios_pui8LRAM);
		
        /* д�볤�� */
        ptr_spi_buf[3] = length;

		for(i=0;i<length;i++)
			ptr_spi_buf[i+4] = data[i];

		ret = 0;
    }
    return ret;
}


/*****************************************************************************
 * �� �� �� : transfer_driver_rx_data
 * �������� : 
    ��������յ����ݣ������͸�Ӧ�ó���
 * ������� : 
 	data:����ָ��
 	length:���ݳ���
 * ������� : ��
 * �� �� ֵ : 
  	0:�����������
 	1:�������ݹ����������ж�һ���ֲ���
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
static uint8_t transfer_driver_rx_data(uint8_t *data,uint16_t length)
{
	uint8_t ret=0;
	
	if(length > RX_BUFFER_SIZE)
	{
		length = RX_BUFFER_SIZE;
		ret = 1;
	}

	transfer_profile_receive_parse(data,length);
	return ret;
}


/*****************************************************************************
 * �� �� �� : transfer_driver_init
 * �������� : 
    SPIs ������ʼ��
 * ������� : ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
static void transfer_driver_init(void)
{
    am_hal_ios_config_t sIOSConfigSPI =
    {
        // Configure the IOS in SPI mode.
        .ui32InterfaceSelect = AM_HAL_IOS_USE_SPI | AM_HAL_IOS_SPIMODE_0,
        // Eliminate the "read-only" section, so an external host can use the
        // entire "direct write" section.
        .ui32ROBase = 0x78,
        // Set the FIFO base to the maximum value, making the "direct write"
        // section as big as possible.
        .ui32FIFOBase = 0x78,
        // We don't need any RAM space, so extend the FIFO all the way to the end
        // of the LRAM.
        .ui32RAMBase = 0x100,
    };

    /* ��ʼ��IOM 1ΪMASTER 0ΪSLAVE */
    am_hal_gpio_pin_config(0, AM_HAL_PIN_0_SLSCK);
    am_hal_gpio_pin_config(1, AM_HAL_PIN_1_SLMISO);
    am_hal_gpio_pin_config(2, AM_HAL_PIN_2_SLMOSI);
    am_hal_gpio_pin_config(3, AM_HAL_PIN_3_SLnCE);

    am_hal_ios_config(&sIOSConfigSPI);

    /* ʹ��IO�ж� */
    am_hal_ios_access_int_clear(AM_HAL_IOS_ACCESS_INT_ALL);
    am_hal_ios_access_int_enable(AM_HAL_IOS_ACCESS_INT_03);
    am_hal_ios_access_int_enable(AM_HAL_IOS_ACCESS_INT_00);

    am_hal_ios_int_clear(AM_HAL_IOS_INT_ALL);
    am_hal_ios_int_enable(AM_HAL_IOS_INT_FSIZE);

    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSACC);
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSLAVE);

    /* �ж�Nordic������ */
    am_hal_gpio_pin_config(SPI_TX_PIN, AM_HAL_PIN_OUTPUT);
    am_hal_gpio_out_bit_clear(SPI_TX_PIN);
    /* �������� */
    am_hal_gpio_pin_config(SPI_RX_PIN, AM_HAL_PIN_INPUT);

    /* �޸�IOSACC���ж����ȼ�,������ж��в��ܵ��ò���ϵͳ����! */
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_IOSACC, configMAX_SYSCALL_INTERRUPT_PRIORITY);
}


/*****************************************************************************
 * �� �� �� : transfer_driver_tx_timer_callback
 * �������� : 
    ע��һ���������ݶ�ʱ���Ļص����������ж�������ڷ��Ͷ���ʱ�����������ʱ�ص�
    ��ʱȥ��������
 * ������� : 
 	xTimer:��ʹ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
static void transfer_driver_tx_timer_callback(TimerHandle_t xTimer)
{
	g_transmit_flg = 1;
	xSemaphoreGive(xSemaphore_driver_tx_rx);
}


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
void transfer_driver_task(void *pvParameters)
{
	uint8_t status = 0;
	
    QPRINTF("transfer_task start..\r\n");
	transfer_driver_init();
	transfer_profile_init();

	g_transmit_flg = 0;

	/* ��д�жϼ����ź��� */
    xSemaphore_driver_tx_rx = xSemaphoreCreateCounting(3, 1);
	xSemaphore_receive_data = xSemaphoreCreateBinary();

	/* ����һ����ʱ��,����Ϊ100ms */
    xTimers_100ms = xTimerCreate("Timer", 10, pdFALSE, (void *)HANDLE_100MS_TIMER, transfer_driver_tx_timer_callback);
    	
	while(1)
	{
		/* �˴������߳� */
        xSemaphoreTake(xSemaphore_driver_tx_rx, portMAX_DELAY);

		status = transfer_profile_transmit_loop();

		if(status && g_transmit_flg ==1)
		{
			g_transmit_flg = 0;
			xTimerStart(xTimers_100ms, 0);
		}
	}
}


/*****************************************************************************
 * �� �� �� : am_ioslave_acc_isr
 * �������� : 
    SPIs �жϻص���������
 * ������� : ��
 * ������� : ��
 * �� �� ֵ : ��
 * �޸���ʷ : ��
 * ˵    �� : ��
*****************************************************************************/
void am_ioslave_acc_isr(void)
{
    uint32_t ui32Status;
    uint8_t *pui8Src;
	uint8_t length;

    portBASE_TYPE xHigherPriorityTaskWoken;

    pui8Src = (uint8_t *)(am_hal_ios_pui8LRAM);

    //
    // Check to see what caused this interrupt, then clear the bit from the
    // interrupt register.
    //
    ui32Status = am_hal_ios_access_int_status_get(false);
    am_hal_ios_access_int_clear(ui32Status);
	
    if(ui32Status & AM_HAL_IOS_ACCESS_INT_00)
    {
		if(pui8Src[0] == 'R')
        {
            QPRINTF("spis send data!\n");
            /* ���ݷ�����,�������� */
            am_hal_gpio_out_bit_clear(SPI_TX_PIN);
		}
    }
    else if(ui32Status & AM_HAL_IOS_ACCESS_INT_03)
    {
		length = pui8Src[3];
		
		transfer_driver_rx_data(&pui8Src[4],length);

		/* ������ȼ��������л�,��֤�����ȼ��������� */
    	xSemaphoreGiveFromISR(xSemaphore_driver_tx_rx, &xHigherPriorityTaskWoken);
    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }	
}

