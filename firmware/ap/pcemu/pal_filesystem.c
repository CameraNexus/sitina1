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
#include "pal_filesystem.h"

int pal_fs_init() {
    // nothing need to be done on Linux
    return 0;
}

void pal_fs_deinit() {

}

// Open a file, return pointer when success, NULL on error
File *pal_fs_open(const char *path, OpenMode mode) {
    char *fopenmode;
    switch (mode)
    {
    case OM_READ:
        fopenmode = "r";
        break;
    case OM_WRITE:
        fopenmode = "w+";
        break;
    case OM_RW:
        fopenmode = "r+";
        break;
    case OM_CREATE:
        fopenmode = "wx";
        break;
    case OM_APPEND:
        fopenmode = "a+";
        break;
    default:
        fprintf(stderr, "Unsupported file open mode!\n");
        return NULL;
        break;
    }
    return fopen(path, fopenmode);
}

// Read from a file, return bytes read, negative on error
int pal_fs_read(File *fp, void *dst, size_t count) {
    return fread(dst, 1, count, fp);
}

// Write to a file, return bytes written, negative on error
int pal_fs_write(File *fp, void *src, size_t count) {
    return fwrite(src, 1, count, fp);
}

// Get current seek, negative on error
int pal_fs_tell(File *fp) {
    return ftell(fp);
}

// Get file size, negative on error
int pal_fs_size(File *fp) {
    size_t seek = ftell(fp);
    fseek(fp, 0, SEEK_END);
    int size = (int)ftell(fp);
    fseek(fp, seek, SEEK_SET);
    return size;
}

// Set seek, returns the new actual seek, negative on error.
int pal_fs_seek(File *fp, size_t seek) {
    return fseek(fp, seek, SEEK_SET);
}

// Close file
int pal_fs_close(File *fp) {
    return fclose(fp);
}

// Open a directory
Directory *pal_fs_opendir(char *path) {
    return opendir(path);
}

// Read directoy file
int pal_fs_readdir(Directory *dp, FileInfo* fno) {
    struct dirent *ent = readdir(dp);
    if (ent == NULL)
        return -1;
    strcpy(fno->fname, ent->d_name);
    if (ent->d_type == DT_REG)
        fno->type = FT_REGULAR;
    else if (ent->d_type == DT_DIR)
        fno->type = FT_DIRECTORY;
    else
        fno->type = FT_OTHER;
    return 0;
}

// Close a directory
int pal_fs_closedir(Directory *dp) {
    return closedir(dp);
}

// Change current directory 
int pal_fs_chdir(char *path) {
    return chdir(path);
}
