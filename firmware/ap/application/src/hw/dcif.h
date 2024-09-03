//
// dcif.h: Digital camera I/F peripheral driver
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
#pragma once

#include "ccd_timing.h"

#define CAM_DMA_BURSTLEN         8   // in 64-bit words
#define CAM_DMA_MAXINFLIGHT      4   // Max number of outstanding requests            
#define CAM_VBLK        (1)
#define CAM_VACT        (CCD_LINES - 3)
#define CAM_HBLK        ((CCD_HBLK_LENGTH + 16) * 2 + 15)
#define CAM_HACT        (CCD_LINE_PIXCNT * 2)
#define CAM_BUFSIZE     (CAM_VACT * CAM_HACT * 2)
#define CAM_BUFALIGN    (CAM_DMA_BURSTLEN * 8)
#if ((CAM_BUFSIZE % CAM_BUFALIGN) != 0)
#error "Buffer not aligned"
#endif

#define CAM_DRAFT_VBLK      (1)
#define CAM_DRAFT_VACT      (CCD_LINES / 3 - 1)
#define CAM_DRAFT_BUFSIZE   (CAM_DRAFT_VACT * CAM_HACT * 2)
#if ((CAM_DRAFT_BUFSIZE % CAM_BUFALIGN) != 0)
#error "Draft buffer not aligned"
#endif

void dcif_init(void);
void dcif_engage(void);
uint8_t *dcif_waitnextbuffer(void);
