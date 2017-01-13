//*****************************************************************************
//
//! @file rtc_ctl.c
//!
//! @brief Functions for controlling the RTC.
//!
//! These functions help the low-level operation of the RTC in an RTOS context.
//
// fixme these will need language localization for China.
//
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
// This is part of revision 1.1.1-mbrd of the AmbiqSuite Development Package.
//
//*****************************************************************************


#include <stdint.h>
#include <stdbool.h>

#include "am_mcu_apollo.h"
#include "am_devices.h"
#include "am_bsp.h"

#include "am_util.h"

#include "rtc_ctl.h"




//*****************************************************************************
//
// Local Epoch Validity flag. Not valid until Bluetooh Time profile has been
// used to set RTC epoch to current time of day from a GPS derived source.
//
//*****************************************************************************
bool g_bRTCEpochValid = false;

//*****************************************************************************
//
// Local string arrays to index Days and Months with the values returned by the RTC.
//
//*****************************************************************************
static const char *pcWeekday[] =
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Invalid day"
};
static const char *pcMonth[] =
{
    "January",
    "February",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December",
    "Invalid month"
};


//*****************************************************************************
//
// Local Support function:
// toVal() converts a string to an ASCII value as an integer.
//
//*****************************************************************************
static int
toVal(char *pcAsciiStr)
{
    int iRetVal = 0;
    iRetVal += pcAsciiStr[1] - '0';
    iRetVal += pcAsciiStr[0] == ' ' ? 0 : (pcAsciiStr[0] - '0') * 10;
    return iRetVal;
}

//*****************************************************************************
//
// Local Support function:
// mthToIndex() converts a string indicating a month to an index value.
// The return value is a value 0-12, with 0-11 indicating the month given
// by the string, and 12 indicating that the string is not a month.
//
//*****************************************************************************
static int
mthToIndex(char *pcMon)
{
    int idx;
    for (idx = 0; idx < 12; idx++)
    {
        if ( am_util_string_strnicmp(pcMonth[idx], pcMon, 3) == 0 )
        {
            return idx;
        }
    }
    return 12;
}

//*****************************************************************************
//
// RTC setup function.
//
//*****************************************************************************
void
rtc_ctl_init(void)
{

    am_hal_rtc_osc_select(AM_HAL_RTC_OSC_XT);
    am_hal_clkgen_osc_start(AM_HAL_CLKGEN_OSC_XT);
    am_hal_clkgen_osc_start(AM_HAL_CLKGEN_OSC_LFRC);

    am_hal_rtc_osc_enable();
}

//*****************************************************************************
//
// RTC get the hours and minutes as 5 character ASCII HH:MM.
//
//*****************************************************************************
void
rtc_ctl_hours_minutes_get(char *pcPtr, bool bUse12HrClock)
{
    am_hal_rtc_time_t hal_time;
    uint8_t ui8Work;

    //
    // Get current time from the RTC
    //
    am_hal_rtc_time_get(&hal_time);

    //
    // extract hours
    //

    //
    // Adjust for 12 hour mode
    //
    if(bUse12HrClock && (hal_time.ui32Hour > 12) ) hal_time.ui32Hour -= 12;

    //
    // Format the hour part of the string.
    //
    ui8Work = hal_time.ui32Hour / 10;
    pcPtr[0] = (ui8Work == 0) ? ' ' : ui8Work + 0x30;
    pcPtr[1] = (hal_time.ui32Hour % 10) + 0x30;


    //
    // Insert separator
    //
    pcPtr[2] = ':';

    //
    // Extract Minutes.
    //
    pcPtr[3] = (hal_time.ui32Minute / 10) + 0x30;
    pcPtr[4] = (hal_time.ui32Minute % 10) + 0x30;
    pcPtr[5] = ':';                               
    pcPtr[6] = (hal_time.ui32Second / 10) + 0x30; 
    pcPtr[7] = (hal_time.ui32Second % 10) + 0x30; 
    pcPtr[8] = 0;                                 
}

//*****************************************************************************
//
// RTC get the month as an ascii string
//
//*****************************************************************************
const char *
rtc_ctl_month_get(void)
{
    am_hal_rtc_time_t hal_time;

    //
    // Get current time from the RTC
    //
    am_hal_rtc_time_get(&hal_time);

    //
    //
    //
    return pcMonth[hal_time.ui32Month];
}

//*****************************************************************************
//
// RTC get the weekday as an ascii string
//
//*****************************************************************************
const char *
rtc_ctl_weekday_get(void)
{
    am_hal_rtc_time_t hal_time;

    //
    // Get current time from the RTC
    //
    am_hal_rtc_time_get(&hal_time);

    //
    //
    //
    return pcWeekday[hal_time.ui32Weekday];
}
