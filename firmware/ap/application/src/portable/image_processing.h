//
// Sitina 1
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
#pragma once

#include "ccd_timing.h"

// 3 quality levels:
// draft - low res input, low res input
// preview - full res input, med res output
// fine - full res input, full res output

#define PROC_IN_WIDTH       (CCD_LINE_PIXCNT * 2)
#define PROC_IN_SKIP        (28)
#define PROC_IN_SKIP_END    (PROC_IN_WIDTH - PROC_PREVIEW_WIDTH * 2 - PROC_IN_SKIP)
#define PROC_IN_SKIP_LINE   (1)
#define PROC_FULL_HEIGHT    (CCD_ACTIVE_LINES + CCD_BUF_LINES_U + CCD_BUF_LINES_D)
#define PROC_FULL_WIDTH     ((CCD_ACTIVE_PIX + 12 * 2) * 2)
#define PROC_PREVIEW_HEIGHT (PROC_FULL_HEIGHT / 2)
#define PROC_PREVIEW_WIDTH  (PROC_FULL_WIDTH / 2 - 2)
#define PROC_DRAFT_HEIGHT   (PROC_PREVIEW_HEIGHT / 4)
#define PROC_DRAFT_WIDTH    (PROC_PREVIEW_WIDTH / 4)

// temp, to be removed
#define PROC_CAM_VACT       (CCD_LINES - 3)
#define PROC_CAM_HBLK       ((CCD_HBLK_LENGTH + 16) * 2 + 15)
#define PROC_CAM_HACT       (CCD_LINE_PIXCNT * 2)
#define PROC_CAM_BUFSIZE    (PROC_CAM_VACT * PROC_CAM_HACT * 2)

#if (PROC_IN_SKIP_END < 0)
#error "OFFSET TOO LARGE"
#endif

void ip_init(void);
void ip_filter_draft_image(uint16_t *in, uint32_t *out, uint8_t *histogram);
void ip_filter_preview_image(uint16_t *in, uint32_t *out, uint8_t *histogram);
