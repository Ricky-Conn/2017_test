//*****************************************************************************
//
//! @file freertos_fit.c
//!
//! @brief Example of the exactle_fit app running under FreeRTOS.
//!
//! This example implements a BLE heart rate sensor within the FreeRTOS
//! framework. To save power, this application is compiled without print
//! statements by default. To enable them, add the following project-level
//! macro definitions.
//!
//! AM_DEBUG_PRINTF
//! WSF_TRACE_ENABLED=1
//!
//! If enabled, debug messages will be sent over ITM.
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
// This is part of revision v1.1.1-703-g603890f-stable of the AmbiqSuite Development Package.
//
//*****************************************************************************

//*****************************************************************************
//
// This application has a large number of common include files. For
// convenience, we'll collect them all together in a single header and include
// that everywhere.
//
//*****************************************************************************
#include "freertos_fit.h"
#include "transfer_driver.h"
#include "transfer_task.h"
//*****************************************************************************
//
// Initializes all tasks
//
//*****************************************************************************
void
run_tasks(void)
{
    //
    // Set some interrupt priorities before we create tasks or start the scheduler.
    //
    // Note: Timer priority is handled by the FreeRTOS kernel, so we won't
    // touch it here.
    //
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_UART, configMAX_SYSCALL_INTERRUPT_PRIORITY);
    am_hal_interrupt_priority_set(AM_HAL_INTERRUPT_GPIO, configMAX_SYSCALL_INTERRUPT_PRIORITY);

    //
    // Create essential tasks.
    //
    xTaskCreate(sleep_task, "Sleep", 64, 0, tskIDLE_PRIORITY, &xSleepTask);
    xTaskCreate(transfer_driver_task, "transfer_driver", 128, 0, 3, &x_transfer_drviver_task);

	xTaskCreate(transfer_task, "transfer task", 256, 0, 2, &x_transfer_task);

    //
    // Start the scheduler.
    //
    vTaskStartScheduler();
}


//*****************************************************************************
//
// Main Function
//
//*****************************************************************************
int
main(void)
{
    //
    // Set the clock frequency
    //
    am_hal_clkgen_sysclk_select(AM_HAL_CLKGEN_SYSCLK_MAX);

    //
    // Initialize the board.
    //
    am_hal_mcuctrl_bucks_enable();
    am_hal_vcomp_disable();
    am_hal_mcuctrl_bandgap_disable();
    //
    // Enable printing to the console.
    //
	SEGGER_RTT_Init();
    QPRINTF("FreeRTOS Fit Example\n");

    //
    // Run the application.
    //
    run_tasks();

    while (1);
}

