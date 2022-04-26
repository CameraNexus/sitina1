/*
 * Copyright 2016-2021 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of NXP Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file    MIMXRT1052_Project.c
 * @brief   Application entry point.
 */
#include <stdio.h>
#include "board.h"
#include "peripherals.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "MIMXRT1052.h"
#include "fsl_gpio.h"
#include "fsl_elcdif.h"
#include "fsl_pwm.h"
#include "fsl_cache.h"
#include "umm_malloc.h"
#include "syslog.h"
#include "usb_support.h"
#include "nano_shell.h"
#include "fb.h"
#include "lcd.h"
#include "storage.h"
#include "ff.h"
#include "ui.h"
#include "afe.h"
#include "csi.h"

uint16_t *camera_buffer = (uint16_t *)0x80000000;

/*
 * @brief   Application entry point.
 */
int main(void) {

    bool result;

    /* Init board hardware. */
    BOARD_ConfigMPU();
    BOARD_InitBootPins();
    BOARD_InitBootClocks();
    BOARD_InitBootPeripherals();
    BOARD_InitSEMC();

    //umm_init();
    syslog_init();
    usbsup_init();

    /*lcd_init();
    memset(framebuffer, 0x00, 614408);
    fb_init();
    lcd_set_bl(50);

    ui_init();*/
    /*uint8_t *rdptr = gImage_test;
    uint8_t *wrptr = framebuffer;
    for (int i = 0; i < 614408/2; i++) {
        uint8_t i1 = *rdptr++;
        uint8_t i2 = *rdptr++;
        *wrptr++ = i2;
        *wrptr++ = i1;
    }*/
    /*uint16_t *wrptr = framebuffer;
    for (int i = 0; i < 640; i++) {
        int c6 = i / 10;
        int c5 = c6 / 2;
        uint16_t c = (c5 << 11) | (c6 << 5) | (c5);
        for (int j = 0; j < 480; j++) {
            *wrptr++ = c;
        }
    }*/
    //memcpy(framebuffer, gImage_test, 614408);

    // Initialize AFE
/*    afe_init();
    SDK_DelayAtLeastUs(1000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

    csi_init();
    csi_submit_empty_buffer((uint8_t *)camera_buffer);

    afe_strobe();

    csi_start();
    afe_start();

    csi_wait_framedone();
    csi_stop();*/

    /*afe_stop();
    DCACHE_InvalidateByRange((uint32_t)camera_buffer, 28*1024*1024);*/

    lcd_init();
    memset(framebuffer, 0x00, 614408);
    fb_init();
    lcd_set_bl(50);

    ui_clear(0x0000);
    ui_disp_string(0, 0, "Waiting for SD card to be inserted...", 0xffff);

    // Initialize SD card
    result = storage_mount();
    if (!result) {
        ui_clear(0xf800);
        ui_disp_string(0, 0, "SD card initialization failed!", 0xffff);
        while (1);
    }

    //ui_init();
    for (int y = 0; y < CCD_FIELD_LINES / 7; y++) {
        for (int x = 0; x < CCD_LINE_LENGTH * 2 / 7; x++) {
            uint16_t src_pixel = camera_buffer[2000 + y * 7 * (CCD_LINE_LENGTH * 2) + x * 7];
            uint16_t dst_pixel = src_pixel;
            framebuffer[x * FB_WIDTH + y] = dst_pixel;
        }
    }

    /*for (int y = 0; y < 120; y++) {
        for (int x = 0; x < 512; x++) {
            framebuffer[x * FB_WIDTH + y] = (x / 2 / 8) << 11;
            framebuffer[x * FB_WIDTH + y + 120] = (x / 2 / 4) << 5;
            framebuffer[x * FB_WIDTH + y + 240] = (x / 2 / 8);
        }
    }*/

    FIL fp;
    uint32_t bw;
    f_open(&fp, "capture.bin", FA_CREATE_ALWAYS | FA_WRITE);
    f_write(&fp, camera_buffer + 2000, CCD_FIELD_LINES * CCD_LINE_LENGTH * 2, &bw);
    f_close(&fp);

    ui_disp_string(0, 470, "File saved!", 0xffff);

    usbsup_waitconnect();
    // Start the shell
    nano_shell_loop(NULL);
    return 0 ;
}
