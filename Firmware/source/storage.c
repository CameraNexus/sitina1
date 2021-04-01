//
// storage.c: Storage related helper functions
//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
#include "fsl_common.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "fsl_sd.h"
#include "syslog.h"
#include "storage.h"
#include "sdmmc_config.h"

__attribute__((section(".ocram"))) FATFS storage_fs;

void storage_mount(void) {
    const TCHAR driverName[3U] = {SDDISK + '0', ':', '/'};

    memset((void *)&storage_fs, 0, sizeof(storage_fs));

    BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success) {
        syslog_printf("SD host init fail\n");
        return;
    }
    /* power off card */
    SD_SetCardPower(&g_sd, false);
    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success) {
        syslog_printf("Card inserted\n");
        /* power on the card */
        SD_SetCardPower(&g_sd, true);
    }
    else {
        syslog_printf("Card detect fail\n");
        return;
    }

    if (f_mount(&storage_fs, driverName, 0)) {
        syslog_printf("Mount volume failed\n");
        return;
    }

    if (f_chdrive((char const *)driverName)) {
        syslog_printf("Change drive failed\n");
        return;
    }

    syslog_printf("Storage initialized\n");
}
