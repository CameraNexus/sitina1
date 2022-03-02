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
#include "umm_malloc.h"
#include "syslog.h"
#include "usb_support.h"
#include "nano_shell.h"
#include "fb.h"
#include "lcd.h"
#include "storage.h"
#include "ui.h"
#include "afe.h"

extern const unsigned char gImage_test[614408];

/*
 * @brief   Application entry point.
 */
int main(void) {

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

    // Initialize SD card
    //storage_mount();

    // Initialize AFE
    afe_init();
    SDK_DelayAtLeastUs(1000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    afe_start();

    usbsup_waitconnect();
    // Start the shell
    nano_shell_loop(NULL);
    return 0 ;
}
