//
// Copyright (c) 2024 Wenting Zhang
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
#include <stdint.h>
#include <stdbool.h>
#include "mu_platform.h"
#include "ccdtg.h"

static uint32_t ctrl_val;

void ccdtg_init() {
    ctrl_val =
        (1 << CCDTG_CTRL_EMBED_ESHUT_OFFSET) |
        (0 << CCDTG_CTRL_VSKIP_OFFSET) |
        (1 << CCDTG_CTRL_CCD_OEN_OFFSET) |
        (1 << CCDTG_CTRL_SYNC_OEN_OFFSET);
    // ctrl_val =
    //     (1 << CCDTG_CTRL_EMBED_ESHUT_OFFSET) |
    //     (6 << CCDTG_CTRL_VSKIP_OFFSET) |
    //     (1 << CCDTG_CTRL_CCD_OEN_OFFSET) |
    //     (1 << CCDTG_CTRL_SYNC_OEN_OFFSET);
    *CCDTG_CTRL = ctrl_val;
    *CCDTG_ESHUT_LINE = 0;
}

void ccdtg_start() {
    *CCDTG_CTRL = ctrl_val | 0x1;
}

void ccdtg_stop() {
    *CCDTG_CTRL = ctrl_val;
}
