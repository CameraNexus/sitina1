//
// SitinaNe
// Copyright 2023 Wenting Zhang, Anhang Li
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
// File : effusus_rawio.h
// Brief: Raw CCD Data Post-Processing - IO

#include "effusus_rawio.h"

#include <stdio.h>

long read_binary_file(const char* fn, uint8_t* buf, long length_limit){
    FILE* fp;
    fp = fopen(fn, "rb");
    if(!fp) {
        fprintf(stderr, "EFFUSUS - cannot open file: %s\n", fn);
        return -1;
    }
    // Get file length
    fseek(fp, 0, SEEK_END);
    long flen = ftell(fp);
    if(flen>length_limit) {
        fprintf(stderr, "EFFUSUS - binary file too large: %s\n", fn);
        fprintf(stderr, "flen: %d, length_limit: %d\n", flen, length_limit);
        return -1;
    }
    fseek(fp, 0, SEEK_SET);
    if(!buf) {
        fprintf(stderr, "EFFUSUS - loading binary into nullptr: %s\n", fn);
        return -1;
    }
    fread(buf, flen, 1, fp);
    fclose(fp);
    return flen;
};
