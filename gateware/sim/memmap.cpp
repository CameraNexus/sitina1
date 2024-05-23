//
// Sitina simulator
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
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "memmap.h"

static uint64_t *mem = NULL;

void memmap_reset() {
    if (!mem)
        mem = (uint64_t *)malloc(RAM_SIZE);
    assert(mem);
}

static uint64_t get_bitmask(uint8_t mask) {
    uint64_t bm = 0;
    for (int i = 0; i < 8; i++) {
        if (mask & 0x01)
            bm |= (0xffull << i * 8);
        mask >>= 1;
    }
    return bm;
}

static int count_trailing_zero(uint8_t mask) {
    int zeros = 0;
    for (int i = 0; i < 8; i++) {
        if (mask & 0x01)
            break;
        zeros++;
        mask >>= 1;
    }
    return zeros;
}

uint64_t memmap_read(uint64_t addr) {
    // Unaligned access is legal
    addr -= RAM_BASE;
    addr >>= 3;
    if (addr > RAM_SIZE) {
        fprintf(stderr, "Illegal read\n");
        return 0;
    }
    return mem[addr];
}

void memmap_write(uint64_t addr, uint64_t data, uint8_t mask) {
    uint64_t original_addr = addr;

    addr -= RAM_BASE;
    addr >>= 3;
    if (addr > RAM_SIZE) {
        fprintf(stderr, "Illegal write to addr %08lx\n", original_addr);
        return;
    }
    if (mask == 0xff) {
        mem[addr] = data;
    }
    else {
        uint64_t d = mem[addr];
        uint64_t bm = get_bitmask(mask);
        d &= ~bm;
        d |= data & bm;
        mem[addr] = d;
    }
}

void memmap_load(const char *fn, size_t offset) {
    FILE *fp;
    fp = fopen(fn, "rb+");
    if (!fp) {
        fprintf(stderr, "Error: unable to open file %s\n", fn);
        exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    size_t result = fread((void *)((size_t)mem + offset), fsize, 1, fp);
    assert(result == 1);
    fclose(fp);
}
