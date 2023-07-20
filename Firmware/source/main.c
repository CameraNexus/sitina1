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
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "sdmmc_config.h"
#include "sdram.h"
#include "vout.h"
#include "i2c.h"
#include "afe.h"
#include "ccd_timing.h"
#include "csi.h"
#include "ui.h"
#include "power.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define LED_NUMBERS  1U
#define LED_1_INIT() USER_LED_INIT(LOGIC_LED_OFF)
#define LED_1_ON()   USER_LED_ON()
#define LED_1_OFF()  USER_LED_OFF()
#define SHELL_Printf PRINTF
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void Led_Init(void);

static shell_status_t LedControl(shell_handle_t shellHandle, int32_t argc, char **argv);

/*******************************************************************************
 * Variables
 ******************************************************************************/
SHELL_COMMAND_DEFINE(led,
                     "\r\n\"led arg1 arg2\":\r\n Usage:\r\n    arg1: 1|2|3|4...         "
                     "   Led index\r\n    arg2: on|off                Led status\r\n",
                     LedControl,
                     2);

SDK_ALIGN(static uint8_t s_shellHandleBuffer[SHELL_HANDLE_SIZE], 4);
static shell_handle_t s_shellHandle;

extern serial_handle_t g_serialHandle;

AT_NONCACHEABLE_SECTION(static FATFS g_fileSystem); /* File system object */
AT_NONCACHEABLE_SECTION(static FIL imgFil);
/*******************************************************************************
 * Code
 ******************************************************************************/

void Led_Init(void)
{
    LED_1_INIT();
}

static shell_status_t LedControl(shell_handle_t shellHandle, int32_t argc, char **argv)
{
    int32_t kLedIndex = ((int32_t)atoi(argv[1]));
    char *kLedCommand = argv[2];

    /* Check second argument to control led */
    switch (kLedIndex)
    {
#if defined(LED_NUMBERS) && LED_NUMBERS > 0
        case 1:
            if (strcmp(kLedCommand, "on") == 0)
            {
                LED_1_ON();
            }
            else if (strcmp(kLedCommand, "off") == 0)
            {
                LED_1_OFF();
            }
            else
            {
                SHELL_Printf("Control conmmand is wrong!\r\n");
            }
            break;
#endif
#if defined(LED_NUMBERS) && LED_NUMBERS > 1
        case 2:
            if (strcmp(kLedCommand, "on") == 0)
            {
                LED_2_ON();
            }
            else if (strcmp(kLedCommand, "off") == 0)
            {
                LED_2_OFF();
            }
            else
            {
                SHELL_Printf("Control conmmand is wrong!\r\n");
            }
            break;
#endif
#if defined(LED_NUMBERS) && LED_NUMBERS > 2
        case 3:
            if (strcmp(kLedCommand, "on") == 0)
            {
                LED_3_ON();
            }
            else if (strcmp(kLedCommand, "off") == 0)
            {
                LED_3_OFF();
            }
            else
            {
                SHELL_Printf("Control conmmand is wrong!\r\n");
            }
            break;
#endif
#if defined(LED_NUMBERS) && LED_NUMBERS > 3
        case 4:
            if (strcmp(kLedCommand, "on") == 0)
            {
                LED_4_ON();
            }
            else if (strcmp(kLedCommand, "off") == 0)
            {
                LED_4_OFF();
            }
            else
            {
                SHELL_Printf("Control conmmand is wrong!\r\n");
            }
            break;
#endif
        default:
            SHELL_Printf("LED index is wrong\r\n");
            break;
    }
    return kStatus_SHELL_Success;
}

void memtest_test(uint32_t addr, uint32_t size) {
    int size_mb = size / 1048576;
    uint32_t *ptr;
    uint32_t testval;
    uint32_t actual;
    SHELL_Printf("\n");
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
            	SHELL_Printf("Test failed at %p, exp %08x, got %08x\r\n",
                        ptr, testval, actual);
                return;
            }
        }
        SHELL_Printf("\r%d MB tested", i + 1);
    }
    SHELL_Printf("\n");
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

static status_t sdcardWaitCardInsert(void)
{
    BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }
    /* power off card */
    //SD_SetCardPower(&g_sd, false);
    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success)
    {
        PRINTF("\r\nCard inserted.\r\n");
        /* power on the card */
        //SD_SetCardPower(&g_sd, true);
    }
    else
    {
        PRINTF("\r\nCard detect fail.\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

static int MOUNT_SDCard(void)
{
    FRESULT error;
    const TCHAR driverName[3U] = {SDDISK + '0', ':', '/'};

    // clear FATFS manually
    memset((void *)&g_fileSystem, 0, sizeof(g_fileSystem));

    /* Wait for the card insert. */
    if (sdcardWaitCardInsert() != kStatus_Success)
    {
        PRINTF("Card not inserted.\r\n");
        return -1;
    }

    // Mount the driver
    if (f_mount(&g_fileSystem, driverName, 0))
    {
        PRINTF("Mount volume failed.\r\n");
        return -2;
    }

#if (FF_FS_RPATH >= 2U)
    if (f_chdrive((char const *)&driverName[0U]))
    {
        PRINTF("Change drive failed.\r\n");
        return -3;
    }
#endif

    return 0;
}


/*! @brief Main function */
int main(void)
{
    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();

    i2c_init();
    power_init();

    PRINTF("\r\nSitinaNe\r\n");

    vout_init();
    PRINTF("LCD Initialized.\r\n");

    sdram_init();

    PRINTF("SDRAM Initialized.\r\n");
    memset(0x80000000, 0xdd, 64*1024*1024);

    //memtest_test(0x80000000, 64*1024*1024);
    ui_init();

    afe_init();
    PRINTF("AFE Initialized.\r\n");

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

    csi_init();
    csi_submit_empty_buffer((uint8_t *)camera_buffer_0);
    csi_submit_empty_buffer((uint8_t *)camera_buffer_1);

    afe_strobe();

    csi_start();
    afe_start();
    PRINTF("Capture started...\r\n");

    uint32_t seq = 0;
    int i = 0;

    uint16_t *camera_buffer;
    while (1) {
        uint16_t max = 0;
        uint16_t min = 65535;
        csi_wait_framedone();
        uint64_t sum = 0;

        while ((camera_buffer = csi_get_full_buffer()) == NULL);
        DCACHE_InvalidateByRange((uint32_t)camera_buffer, 28*1024*1024);

        uint16_t *fb_ptr = framebuffer;
        for (int y = 0; y < CCD_FIELD_LINES / 7; y++) {
            for (int x = 0; x < CCD_LINE_LENGTH * 2 / 7; x++) {
                uint16_t src_pixel = camera_buffer[2000 + y * 7 * (CCD_LINE_LENGTH * 2) + x * 7];
                if (src_pixel > max) max = src_pixel;
                if (src_pixel < min) min = src_pixel;
                sum += src_pixel;
                uint16_t dst_pixel = (src_pixel >> 11) & 0x1f;
                dst_pixel = (dst_pixel << 11) | (dst_pixel << 6) | dst_pixel;
                //fb_ptr[y * FB_WIDTH + x] = dst_pixel;
                uint32_t dst_x = (x & 0x1) ? (CCD_LINE_LENGTH * 2 / 7 - 1 - x / 2) : x / 2;
                fb_ptr[y * FB_WIDTH + dst_x] = dst_pixel;
            }
        }

//        i++;
//        if (i == 10)
//            break;

        csi_submit_empty_buffer((uint8_t *)camera_buffer);

        sum /= (CCD_FIELD_LINES / 7) * (CCD_LINE_LENGTH * 2 / 7);
        memset(framebuffer + 400*720*2, 0x00, 30*720*2);
        //seq++;
        ui_printf(0, 400, "%d %d %d", min, max, (uint32_t)sum);
    }

    csi_stop();

    afe_stop();
    PRINTF("Capture done.\r\n");

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

    /* Init SHELL */
    s_shellHandle = &s_shellHandleBuffer[0];

    SHELL_Init(s_shellHandle, g_serialHandle, "SHELL>> ");
    /* Add new command to commands list */
    SHELL_RegisterCommand(s_shellHandle, SHELL_COMMAND(led));

    while (1)
    {
#if !(defined(SHELL_NON_BLOCKING_MODE) && (SHELL_NON_BLOCKING_MODE > 0U))
        SHELL_Task(s_shellHandle);
#endif
    }
}
