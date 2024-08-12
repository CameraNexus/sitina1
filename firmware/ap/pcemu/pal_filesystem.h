//
// Sitina1
// Copyright 2023 Wenting Zhang
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
#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>

typedef enum {
    OM_READ,   // r
    OM_WRITE,  // w+
    OM_RW,     // r+
    OM_CREATE, // wx
    OM_APPEND  // a+
} OpenMode;

typedef enum {
    FT_REGULAR,
    FT_DIRECTORY,
    FT_OTHER
} FileType;

typedef FILE File;
typedef DIR Directory;
typedef struct {
    FileType type;
    char fname[256]; // 255 byte LFN + 1 byte
} FileInfo;

int pal_fs_init();
void pal_fs_deinit();
// Open a file, return pointer when success, NULL on error
File *pal_fs_open(const char *path, OpenMode mode);
// Read from a file, return bytes read, negative on error
int pal_fs_read(File *fp, void *dst, size_t count);
// Write to a file, return bytes written, negative on error
int pal_fs_write(File *fp, void *src, size_t count);
// Get current seek, negative on error
int pal_fs_tell(File *fp);
// Get file size, negative on error
int pal_fs_size(File *fp);
// Set seek, returns the new actual seek, negative on error.
int pal_fs_seek(File *fp, size_t seek);
// Close file
int pal_fs_close(File *fp);

// Open a directory
Directory *pal_fs_opendir(char *path);
// Read directoy file
int pal_fs_readdir(Directory *dp, FileInfo* fno);
// Close a directory
int pal_fs_closedir(Directory *dp);
// Change current directory
int pal_fs_chdir(char *path);
