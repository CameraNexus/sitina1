//
// Sitina1
// Copyright 2024 Wenting Zhang
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
#include <stdlib.h>
#include "xparameters.h"	/* SDK generated parameters */
#include "xsdps.h"		/* SD device driver */
#include "xil_printf.h"
#include "ff.h"
#include "xil_cache.h"
#include "xplatform_info.h"
#include "pal_filesystem.h"

static FATFS fatfs;
static TCHAR rootpath[] = "0:/";

int pal_fs_init() {
    uint8_t *tmp = malloc(512*1024);

    FRESULT res;
    res = f_mkfs(rootpath, 0, tmp, 512*1024);
    if (res != FR_OK) {
        printf("Failed to format card %d\n", res);
    }

    res = f_mount(&fatfs, rootpath, 1);
    return (int)res;
}

void pal_fs_deinit() {
    //
}

// Open a file, return pointer when success, NULL on error
File *pal_fs_open(const char *path, OpenMode mode) {
    BYTE fatfsmode;
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
        //printf("Unsupported file open mode!\n");
        return NULL;
        break;
    }
    result = f_open((FIL *)file, path, fatfsmode);
    if (result != FR_OK) {
        printf("fopen failed %d\n", result);
        free(file);
        return NULL;
    }
    else {
        return file;
    }
}

// Read from a file, return bytes read, negative on error
int pal_fs_read(File *fp, void *dst, size_t count) {
    unsigned int bytes;
    FRESULT result = f_read(fp, dst, count, &bytes);
    if (result != FR_OK)
        return -1;
    else
        return bytes;
}

// Write to a file, return bytes written, negative on error
int pal_fs_write(File *fp, void *src, size_t count) {
    unsigned int bytes;
    FRESULT result = f_write(fp, src, count, &bytes);
    if (result != FR_OK) {
        printf("fwrite failed %d\n", result);
        return -1;
    }
    else {
        return bytes;
    }
}

// Get current seek, negative on error
int pal_fs_tell(File *fp) {
    return f_tell(fp);
}

// Get file size, negative on error
int pal_fs_size(File *fp) {
    return f_size(fp);
}

// Set seek, returns the new actual seek, negative on error.
int pal_fs_seek(File *fp, size_t seek) {
    FRESULT result = f_lseek(fp, seek);
    return (result == FR_OK) ? 0 : -1;
}

// Close file
int pal_fs_close(File *fp) {
    FRESULT result = f_close(fp);
    return (result == FR_OK) ? 0 : -1;
}

// Open a directory
Directory *pal_fs_opendir(char *path) {
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
int pal_fs_readdir(Directory *dp, FileInfo* fno) {
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
int pal_fs_closedir(Directory *dp) {
    FRESULT result = f_closedir(dp);
    return (result == FR_OK) ? 0 : -1;
}

// Change current directory 
int pal_fs_chdir(char *path) {
    FRESULT result = f_chdir(path);
    return (result == FR_OK) ? 0 : -1;
}
