//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdlib.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "sdmmc_config.h"
#include "os_filesystem.h"

AT_NONCACHEABLE_SECTION(static FATFS g_fileSystem); /* File system object */

static status_t sdcardWaitCardInsert(void)
{
    BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        printf("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }
    /* power off card */
    //SD_SetCardPower(&g_sd, false);
    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success)
    {
        printf("\r\nCard inserted.\r\n");
        /* power on the card */
        //SD_SetCardPower(&g_sd, true);
    }
    else
    {
        printf("\r\nCard detect fail.\r\n");
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
        printf("Card not inserted.\r\n");
        return -1;
    }

    // Mount the driver
    if (f_mount(&g_fileSystem, driverName, 0))
    {
        printf("Mount volume failed.\r\n");
        return -2;
    }

#if (FF_FS_RPATH >= 2U)
    if (f_chdrive((char const *)&driverName[0U]))
    {
        printf("Change drive failed.\r\n");
        return -3;
    }
#endif

    return 0;
}

int os_fs_init() {
    //MOUNT_SDCard();

    return 0;
}

// Open a file, return pointer when success, NULL on error
File *os_fs_open(char *path, OpenMode mode) {
    BYTE fatfsmode;
    // TODO: This should be allocated in uncacheable area?
    File *file = malloc(sizeof(File));
    FRESULT result;
    switch (mode)
    {
    case OM_READ:
        fatfsmode = FA_READ;
        break;
    case OM_WRITE:
        fatfsmode = FA_CREATE_ALWAYS | FA_READ | FA_WRITE;
        break;
    case OM_RW:
        fatfsmode = FA_READ | FA_WRITE;
        break;
    case OM_CREATE:
        fatfsmode = FA_CREATE_NEW | FA_READ | FA_WRITE;
        break;
    case OM_APPEND:
        fatfsmode = FA_OPEN_APPEND | FA_READ | FA_WRITE;
        break;
    default:
        printf("Unsupported file open mode!\n");
        return NULL;
        break;
    }
    result = f_open((FIL *)file, path, fatfsmode);
    if (result != FR_OK) {
        free(file);
        return NULL;
    }
    else {
        return file;
    }
}

// Read from a file, return bytes read, negative on error
int os_fs_read(File *fp, void *dst, size_t count) {
    uint32_t bytes;
    FRESULT result = f_read(fp, dst, count, &bytes);
    if (result != FR_OK)
        return -1;
    else
        return bytes;
}

// Write to a file, return bytes written, negative on error
int os_fs_write(File *fp, void *src, size_t count) {
    uint32_t bytes;
    FRESULT result = f_write(fp, src, count, &bytes);
    if (result != FR_OK)
        return -1;
    else
        return bytes;
}

// Get current seek, negative on error
int os_fs_tell(File *fp) {
    return f_tell(fp);
}

// Get file size, negative on error
int os_fs_size(File *fp) {
    return f_size(fp);
}

// Set seek, returns the new actual seek, negative on error.
int os_fs_seek(File *fp, size_t seek) {
    FRESULT result = f_lseek(fp, seek);
    return (result == FR_OK) ? 0 : -1;
}

// Close file
int os_fs_close(File *fp) {
    FRESULT result = f_close(fp);
    return (result == FR_OK) ? 0 : -1;
}

// Open a directory
Directory *os_fs_opendir(char *path) {
    Directory *dir = malloc(sizeof(Directory));
    FRESULT result = f_opendir(dir, path);
    if (result == FR_OK) {
        return dir;
    }
    else {
        free(dir);
        return NULL;
    }
}

// Read directoy file
int os_fs_readdir(Directory *dp, FileInfo* fno) {
    FILINFO fatfs_fno;
    FRESULT result = f_readdir(dp, &fatfs_fno);
    if (result != FR_OK) {
        return -1;
    }
    strcpy(fno->fname, fatfs_fno.fname);
    // FT_OTHER is not used for FAT
    if (fatfs_fno.fattrib &= AM_DIR)
        fno->type = FT_DIRECTORY;
    else
        fno->type = FT_REGULAR;
    return 0;
}

// Close a directory
int os_fs_closedir(Directory *dp) {
    FRESULT result = f_closedir(dp);
    return (result == FR_OK) ? 0 : -1;
}

// Change current directory
int os_fs_chdir(char *path) {
    FRESULT result = f_chdir(path);
    return (result == FR_OK) ? 0 : -1;
}
