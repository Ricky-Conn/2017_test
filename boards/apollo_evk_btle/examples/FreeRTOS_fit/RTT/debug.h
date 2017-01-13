#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"

#define DEBUG_ENABLE		(1)

#if DEBUG_ENABLE
#define QPRINTF(...)	SEGGER_RTT_printf(0, ##__VA_ARGS__)

#else
#define QPRINTF(...)

#endif

#endif

