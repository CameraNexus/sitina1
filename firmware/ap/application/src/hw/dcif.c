//
// power.c: Power management related functions
//
// Copyright 2024 Wenting Zhang <zephray@outlook.com>
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
#include <stdint.h>
#include <stdbool.h>
#include "xil_cache.h"
#include "mu_platform.h"
#include "ccd_timing.h"
#include "dcif.h"

uint8_t __aligned(64) cam_buf0[CAM_BUFSIZE];
uint8_t __aligned(64) cam_buf1[CAM_BUFSIZE];

static uint32_t ctrl_val;

void dcif_init(void) {
    *DCIF_DMACTL = (CAM_DMA_MAXINFLIGHT << 8) | (CAM_DMA_BURSTLEN);
    *DCIF_VBLK = CAM_VBLK;
    *DCIF_VACT = CAM_VACT;
    *DCIF_HBLK = CAM_HBLK;
    *DCIF_HACT = CAM_HACT;
    *DCIF_STARTADDR0_L = (uint32_t)cam_buf0;
    *DCIF_ENDADDR0_L = (uint32_t)cam_buf0 + CAM_BUFSIZE;
    *DCIF_STARTADDR1_L = (uint32_t)cam_buf1;
    *DCIF_ENDADDR1_L = (uint32_t)cam_buf1 + CAM_BUFSIZE;
    ctrl_val =
        (1 << DCIF_CTRL_USE_VSYNC_OFFSET) |
        (1 << DCIF_CTRL_USE_HSYNC_OFFSET) |
        (1 << DCIF_CTRL_VSYNC_POL_OFFSET) |
        (1 << DCIF_CTRL_HSYNC_POL_OFFSET);
    *DCIF_PCTL = ctrl_val;
}

void dcif_engage(void) {
    *DCIF_PCTL = ctrl_val | 0x1;
}

uint8_t *dcif_waitnextbuffer(void) {
    *DCIF_FBSWAP = 1;
    while (*DCIF_FBSWAP);
    uint32_t buf_addr = *DCIF_STARTADDR1_L;
    //Xil_DCacheInvalidateRange((intptr_t)buf_addr, CAM_BUFSIZE);
    return (uint8_t *)buf_addr;
}
