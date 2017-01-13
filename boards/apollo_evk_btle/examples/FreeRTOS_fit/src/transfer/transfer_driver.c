/***********************************************************************************
 * 文 件 名   : transfer_driver.c
 * 创 建 者   : liuyuanbin
 * 创建日期   : 2017年1月4日
 * 版权说明   : Copyright (c) 2016-2025   广东乐心医疗电子股份有限公司
 * 文件描述   : 进行通信协议驱动层的编写，目前使用SPI驱动，以后需要修改通信接口的时候
 				只需要修改这个驱动文件，
 * 修改历史   : 
***********************************************************************************/

/*******************   头文件  ***************************/
#include "transfer_driver.h"
#include "transfer_profile.h"


/*******************  全局变量 ***************************/
/* 软定时器handl */
#define HANDLE_100MS_TIMER 1


/*******************   宏定义  ***************************/
TaskHandle_t x_transfer_drviver_task;
xSemaphoreHandle xSemaphore_driver_tx_rx;
xSemaphoreHandle xSemaphore_receive_data;
TimerHandle_t xTimers_100ms;
uint8_t g_transmit_flg = 0;


/*****************************************************************************
 * 函 数 名 : transfer_driver_bus_status
 * 函数功能 : 
    发送数据前，先去获取传输数据总线的状态
 * 输入参数 : 无
 * 输出参数 : 无
 * 返 回 值 : 
  	0:忙
 	1:空闲
 * 修改历史 : 无
 * 说    明 : 定义检查IO口，空闲为低，忙为高
*****************************************************************************/
static uint8_t transfer_driver_bus_status(void)
{
    if(am_hal_gpio_input_bit_read(SPI_RX_PIN))
    {
        /* 表明为高,正忙 */
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
uint8_t transfer_driver_tx_data(uint8_t *data,uint16_t length)
{
    uint8_t ret;
    unsigned short i;
    unsigned char *ptr_spi_buf;
    
    ret = 0xFF;

    if(transfer_driver_bus_status())
    {
        ptr_spi_buf = (unsigned char *)(am_hal_ios_pui8LRAM);
		
        /* 写入长度 */
        ptr_spi_buf[3] = length;

		for(i=0;i<length;i++)
			ptr_spi_buf[i+4] = data[i];

		ret = 0;
    }
    return ret;
}


/*****************************************************************************
 * 函 数 名 : transfer_driver_rx_data
 * 函数功能 : 
    驱动层接收到数据，并发送给应用程序。
 * 输入参数 : 
 	data:数据指针
 	length:数据长度
 * 输出参数 : 无
 * 返 回 值 : 
  	0:接收数据完成
 	1:接收数据过长，进行切断一部分操作
 * 修改历史 : 无
 * 说    明 : 无
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
 * 函 数 名 : transfer_driver_init
 * 函数功能 : 
    SPIs 驱动初始化
 * 输入参数 : 无
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
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

    /* 初始化IOM 1为MASTER 0为SLAVE */
    am_hal_gpio_pin_config(0, AM_HAL_PIN_0_SLSCK);
    am_hal_gpio_pin_config(1, AM_HAL_PIN_1_SLMISO);
    am_hal_gpio_pin_config(2, AM_HAL_PIN_2_SLMOSI);
    am_hal_gpio_pin_config(3, AM_HAL_PIN_3_SLnCE);

    am_hal_ios_config(&sIOSConfigSPI);

    /* 使能IO中断 */
    am_hal_ios_access_int_clear(AM_HAL_IOS_ACCESS_INT_ALL);
    am_hal_ios_access_int_enable(AM_HAL_IOS_ACCESS_INT_03);
    am_hal_ios_access_int_enable(AM_HAL_IOS_ACCESS_INT_00);

    am_hal_ios_int_clear(AM_HAL_IOS_INT_ALL);
    am_hal_ios_int_enable(AM_HAL_IOS_INT_FSIZE);

    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSACC);
    am_hal_interrupt_enable(AM_HAL_INTERRUPT_IOSLAVE);

    /* 中断Nordic的引脚 */
    am_hal_gpio_pin_config(SPI_TX_PIN, AM_HAL_PIN_OUTPUT);
    am_hal_gpio_out_bit_clear(SPI_TX_PIN);
    /* 输入引脚 */
    am_hal_gpio_pin_config(SPI_RX_PIN, AM_HAL_PIN_INPUT);

    /* 修改IOSACC的中断优先级,否则该中断中不能调用操作系统函数! */
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_IOSACC, configMAX_SYSCALL_INTERRUPT_PRIORITY);
}


/*****************************************************************************
 * 函 数 名 : transfer_driver_tx_timer_callback
 * 函数功能 : 
    注册一个发送数据定时器的回调函数，当有多包数据在发送队列时，调用这个定时回调
    定时去发送数据
 * 输入参数 : 
 	xTimer:无使用
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
*****************************************************************************/
static void transfer_driver_tx_timer_callback(TimerHandle_t xTimer)
{
	g_transmit_flg = 1;
	xSemaphoreGive(xSemaphore_driver_tx_rx);
}


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
void transfer_driver_task(void *pvParameters)
{
	uint8_t status = 0;
	
    QPRINTF("transfer_task start..\r\n");
	transfer_driver_init();
	transfer_profile_init();

	g_transmit_flg = 0;

	/* 被写中断计数信号量 */
    xSemaphore_driver_tx_rx = xSemaphoreCreateCounting(3, 1);
	xSemaphore_receive_data = xSemaphoreCreateBinary();

	/* 创建一个定时器,周期为100ms */
    xTimers_100ms = xTimerCreate("Timer", 10, pdFALSE, (void *)HANDLE_100MS_TIMER, transfer_driver_tx_timer_callback);
    	
	while(1)
	{
		/* 此处阻塞线程 */
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
 * 函 数 名 : am_ioslave_acc_isr
 * 函数功能 : 
    SPIs 中断回调函数处理。
 * 输入参数 : 无
 * 输出参数 : 无
 * 返 回 值 : 无
 * 修改历史 : 无
 * 说    明 : 无
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
            /* 数据发送完,拉低引脚 */
            am_hal_gpio_out_bit_clear(SPI_TX_PIN);
		}
    }
    else if(ui32Status & AM_HAL_IOS_ACCESS_INT_03)
    {
		length = pui8Src[3];
		
		transfer_driver_rx_data(&pui8Src[4],length);

		/* 检查优先级并进行切换,保证高优先级任务运行 */
    	xSemaphoreGiveFromISR(xSemaphore_driver_tx_rx, &xHigherPriorityTaskWoken);
    	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
    }	
}

