/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 *
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_component_serial_manager.h"
#include "fsl_shell.h"
#include "sdram.h"
#include "vout.h"
#include "i2c.h"
#include "afe.h"
#include "ccd_timing.h"
#include "csi.h"
#include "vt.h"
#include "power.h"
#include "app_main.h"
#include "ff.h"
#include "diskio.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

void memtest_test(uint32_t addr, uint32_t size) {
    int size_mb = size / 1048576;
    uint32_t *ptr;
    uint32_t testval;
    uint32_t actual;
    printf("\n");
    for (int i = 0; i < size_mb; i++) {
        ptr = (uint32_t *)(addr + i * 1048576);
        for (int j = 0; j < 128 * 1024; j++) {
            testval = ~(uint32_t)ptr;
            *ptr++ = testval;
        }
        ptr = (uint32_t *)(addr + i * 1048576);
        for (int j = 0; j < 128 * 1024; j++) {
            testval = ~(uint32_t)ptr;
            actual = *ptr++;
            if (actual != testval) {
                printf("Test failed at %p, exp %08x, got %08x\r\n",
                        ptr, testval, actual);
                return;
            }
        }
        printf("\r%d MB tested", i + 1);
    }
    printf("\n");
}

/*! @brief Main function */
int main(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    vt_init();
    sdram_init();
    memset(0x80000000, 0x00, 64*1024*1024);
    printf("SitinaNe\r\n");

    i2c_init();
    power_init();

    //memtest_test(0x80000000, 64*1024*1024);

    //i2c_probe(LPI2C4);

    app_main();

    while (1);

}
