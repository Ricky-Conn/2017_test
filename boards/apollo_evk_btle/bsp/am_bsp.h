//*****************************************************************************
//
//! @file am_bsp.h
//!
//! @brief Functions to aid with configuring the GPIOs.
//!
//! @addtogroup BSP Board Support Package (BSP)
//! @addtogroup apollo_evk_btle_sensor_bsp BSP for the Apollo EVK BTLE + SENSOR board
//! @ingroup BSP
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// Copyright (c) 2016, Ambiq Micro
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
// contributors may be used to endorse or promote products derived from this
// software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// This is part of revision v1.1.1-741-g86c627b-mbrd of the AmbiqSuite Development Package.
//
//*****************************************************************************

#ifndef AM_BSP_H
#define AM_BSP_H

#include <stdint.h>
#include <stdbool.h>
#include "am_mcu_apollo.h"
#include "am_devices.h"
#include "am_bsp_gpio.h"

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// HRM peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_PAH8001_PRESENT              0
#define AM_BSP_PAH8002_PRESENT              1

//*****************************************************************************
//
// IOM Mode Definitions for this board.
//
//*****************************************************************************
#define AM_BSP_IOM0_SPI_MODE                0
#define AM_BSP_IOM1_SPI_MODE                1

//*****************************************************************************
//
// BMI160 peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_BMI160_PRESENT               1
#define AM_BSP_BMI160_IOM                   1
#define AM_BSP_BMI160_CS                    0

//*****************************************************************************
//
// ADXL362 peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_ADXL362_IOM                  1
#define AM_BSP_ADXL362_CS                   3
#define AM_BSP_ADXL362_TIMER                1
#define AM_BSP_ADXL362_TIMER_SEG            AM_HAL_CTIMER_TIMERB

//*****************************************************************************
//
// FLASH peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_FLASH_IOM                    1
#define AM_BSP_FLASH_CS                     6

//*****************************************************************************
//
// DISPLAY peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_DISPLAY_LS013B4DN04_PRESENT   0
#define AM_BSP_DISPLAY_QG9696TSWHG04_PRESENT 1
#define AM_BSP_DISPLAY_IOM                   1
#define AM_BSP_DISPLAY_CS                    6
#define AM_BSP_DISPLAY_DC                    31

//*****************************************************************************
//
// L3GD20H peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_L3GD20H_IOM                  1
#define AM_BSP_L3GD20H_CS                   2

//*****************************************************************************
//
// LIS3MDL peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_LIS3MDL_PRESENT              1
#define AM_BSP_LIS3MDL_IOM                  1
#define AM_BSP_LIS3MDL_CS                   1

//*****************************************************************************
//
// RTC peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_RTC_IOM                      1
#define AM_BSP_RTC_CS                       4

//*****************************************************************************
//
// PWM_LED peripheral assignments.
//
//*****************************************************************************
#define AM_BSP_PWM_LED_TIMER                0
#define AM_BSP_PWM_LED_TIMER_SEG            AM_HAL_CTIMER_TIMERB

//*****************************************************************************
//
// Button definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_BUTTONS                  4
extern am_devices_button_t am_bsp_psButtons[AM_BSP_NUM_BUTTONS];

//*****************************************************************************
//
// LED definitions.
//
//*****************************************************************************
#define AM_BSP_NUM_LEDS                     4
extern am_devices_led_t am_bsp_psLEDs[AM_BSP_NUM_LEDS];

//*****************************************************************************
// The 3 sensors (accel, gyro, mag) vary from board to board.
// Their coordinates must be transformed to a common frame of reference and
// the magnetometer must have a board specific soft iron correction applied.
// If sensor fusion is implemented, it uses these values for this board.
//*****************************************************************************

#define AM_BSP_ACCEL_BOARD_ROTATION   0x12, 0x04
#define AM_BSP_GYRO_BOARD_ROTATION    0x12, 0x04
#define AM_BSP_MAG_BOARD_ROTATION     0x06, 0x06
#define AM_BSP_SOFT_IRON_SHIFT_AMOUNT (14)
#define AM_BSP_SOFT_IRON_CORRECTION_MATRIX  {16384,     0,      0, \
                                                 0,  16384,     0, \
                                                 0,     0, 16384}

#define EXPECTED_SP_ACCEL_VALUES {0, 0, -4096}

//*****************************************************************************
//
// UART definitions.
//
//*****************************************************************************
//
// Apollo has a single UART instance.
//
#define AM_BSP_UART_PRINT_INST          0
#define AM_BSP_UART_BTLE_INST           0

//*****************************************************************************
//
// External function definitions.
//
//*****************************************************************************
extern void am_bsp_iom_spi_pins_enable(uint32_t ui32Module);
extern void am_bsp_iom_spi_pins_disable(uint32_t ui32Module);
extern void am_bsp_iom_i2c_pins_enable(uint32_t ui32Module);
extern void am_bsp_iom_i2c_pins_disable(uint32_t ui32Module);
extern void am_bsp_low_power_init(void);
extern void am_bsp_debug_printf_enable(void);
extern void am_bsp_debug_printf_disable(void);
extern void am_bsp_itm_string_print(char *pcString);
extern void am_bsp_uart_string_print(char *pcString);

#ifdef __cplusplus
}
#endif

#endif // AM_BSP_H
//*****************************************************************************
//
// End Doxygen group.
//! @}
//
//*****************************************************************************
