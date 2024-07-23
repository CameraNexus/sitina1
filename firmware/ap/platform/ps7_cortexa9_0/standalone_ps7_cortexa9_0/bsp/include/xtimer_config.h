/******************************************************************************
* Copyright (c) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
#ifndef _XTIMER_CONFIG_H
#define _XTIMER_CONFIG_H



#define XSLEEPTIMER_BASEADDRESS 0xf8f00600
/* #undef XSLEEPTIMER_IS_AXITIMER */
/* #undef XSLEEPTIMER_IS_TTCPS */
#define XSLEEPTIMER_IS_SCUTIMER  
#define XSLEEPTIMER_FREQ	     XPAR_CPU_CORE_CLOCK_FREQ_HZ/2
#define COUNTS_PER_SECOND       XSLEEPTIMER_FREQ
/* #undef XTIMER_IS_DEFAULT_TIMER */
/* #undef XTIMER_DEFAULT_TIMER_IS_MB */

/* #undef XTICKTIMER_BASEADDRESS */
/* #undef XTICKTIMER_IS_AXITIMER */
/* #undef XTICKTIMER_IS_TTCPS */
/* #undef XTICKTIMER_IS_SCUTIMER */
#define XTIMER_NO_TICK_TIMER   1

#endif /* XTIMER_CONFIG_H */
