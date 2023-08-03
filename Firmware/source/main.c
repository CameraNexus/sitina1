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

void lcd_set_color(uint16_t cl) {
	uint16_t *wrptr = (uint16_t *)framebuffer;
	for (int i = 0; i < 720*720; i++) {
		*wrptr++ = cl;
	}
	DCACHE_CleanInvalidateByRange((uint32_t)framebuffer, (720 * 720 * 2));
}

// LCD buffer takes 720*720*2= ~1MB for now, reserve 4MB for LCD

uint16_t *camera_buffer_0 = (uint16_t *)0x80400000;
uint16_t *camera_buffer_1 = (uint16_t *)0x82000000;

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
    printf("SDRAM Initialized.\r\n");

    i2c_init();
    power_init();

    vout_init();
    printf("LCD Initialized.\r\n");

    //memtest_test(0x80000000, 64*1024*1024);

    //i2c_probe(LPI2C4);

    afe_init();
    printf("AFE Initialized.\r\n");

    /* ERR050396
     * Errata description:
     * AXI to AHB conversion for CM7 AHBS port (port to access CM7 to TCM) is by a NIC301 block, instead of XHB400
     * block. NIC301 doesn't support sparse write conversion. Any AXI to AHB conversion need XHB400, not by NIC. This
     * will result in data corruption in case of AXI sparse write reaches the NIC301 ahead of AHBS. Errata workaround:
     * For uSDHC, don't set the bit#1 of IOMUXC_GPR28 (AXI transaction is cacheable), if write data to TCM aligned in 4
     * bytes; No such write access limitation for OCRAM or external RAM
     */
    IOMUXC_GPR->GPR28 &= (~IOMUXC_GPR_GPR28_AWCACHE_USDHC_MASK);

//    int result = MOUNT_SDCard();
//    if (result != 0) {
//        ui_printf(0, 0, "Unable to mount SD card!");
//        while (1);
//    }

//     csi_init();
//     csi_submit_empty_buffer((uint8_t *)camera_buffer_0);
//     csi_submit_empty_buffer((uint8_t *)camera_buffer_1);

//     afe_strobe();

//     csi_start();
//     afe_start();
//     printf("Capture started...\r\n");

//     uint32_t seq = 0;
//     int i = 0;

//     uint16_t *camera_buffer;
//     while (1) {
//         uint16_t max = 0;
//         uint16_t min = 65535;
//         csi_wait_framedone();
//         uint64_t sum = 0;

//         while ((camera_buffer = csi_get_full_buffer()) == NULL);
//         DCACHE_InvalidateByRange((uint32_t)camera_buffer, 28*1024*1024);

//         uint16_t *fb_ptr = framebuffer;
//         for (int y = 0; y < CCD_FIELD_LINES / 7; y++) {
//             for (int x = 0; x < CCD_LINE_LENGTH * 2 / 7; x++) {
//                 uint16_t src_pixel = camera_buffer[2000 + y * 7 * (CCD_LINE_LENGTH * 2) + x * 7];
//                 if (src_pixel > max) max = src_pixel;
//                 if (src_pixel < min) min = src_pixel;
//                 sum += src_pixel;
//                 uint16_t dst_pixel = (src_pixel >> 11) & 0x1f;
//                 dst_pixel = (dst_pixel << 11) | (dst_pixel << 6) | dst_pixel;
//                 //fb_ptr[y * FB_WIDTH + x] = dst_pixel;
//                 uint32_t dst_x = (x & 0x1) ? (CCD_LINE_LENGTH * 2 / 7 - 1 - x / 2) : x / 2;
//                 fb_ptr[y * FB_WIDTH + dst_x] = dst_pixel;
//             }
//         }

// //        i++;
// //        if (i == 10)
// //            break;

//         csi_submit_empty_buffer((uint8_t *)camera_buffer);

//         sum /= (CCD_FIELD_LINES / 7) * (CCD_LINE_LENGTH * 2 / 7);
//         memset(framebuffer + 400*720*2, 0x00, 30*720*2);
//         //seq++;
//         ui_printf(0, 400, "%d %d %d", min, max, (uint32_t)sum);
//     }

//     csi_stop();

//     afe_stop();
//     printf("Capture done.\r\n");

//    // Save to SD card
//    FRESULT error = f_open(&imgFil, "/capture.bin", FA_CREATE_ALWAYS | FA_WRITE);
//    if (error != FR_OK)
//    {
//        memset(framebuffer + 400*720*2, 0x00, 30*720*2);
//        ui_printf(0, 400, "Unable to create file");
//        while(1);
//    }
//
//    uint32_t btw = (CCD_LINE_LENGTH * 2 * 2) * (CCD_FIELD_LINES + 1);
//    uint32_t bw;
//    error = f_write(&imgFil, camera_buffer, btw, &bw);
//    if (error != FR_OK)
//    {
//        memset(framebuffer + 400*720*2, 0x00, 30*720*2);
//        ui_printf(0, 400, "Unable to write file");
//        while(1);
//    }
//
//    f_close(&imgFil);
//
//    memset(framebuffer + 400*720*2, 0x00, 30*720*2);
//    ui_printf(0, 400, "Image write finished.");

    app_main();

}
