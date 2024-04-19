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
// File : effusus.h
// Brief: Raw CCD Data Post-Processing - Top

#ifndef _EFFUSUS_H_
#define _EFFUSUS_H_

#ifdef EFFUSUS_TOP
#define EFFUSUS_G
#else
#define EFFUSUS_G export
#endif /* EFFUSUS_TOP */

// Standard Library
#include <stdbool.h>
#include <stdint.h>

// Effusus Library
#include "effusus_rawio.h"

// Read this from "ccd_timing.h"
typedef struct {
    bool         interleaved;   // Use interleaved ADC readout
    // Timing for normal readout
    unsigned int tvccd_pix;
    unsigned int thd_pix;
    unsigned int dummy_pix;
    unsigned int dark_pix;
    unsigned int buffer_pix;
    unsigned int active_pix;
    unsigned int t3p_pix;
    unsigned int tv3rd_pix;
    unsigned int t3d_pix;
    unsigned int line_length;
    unsigned int hblk_length;
    unsigned int vsg_length;
    unsigned int clpob_begin;
    unsigned int clpob_end;
    // Timing for fast line dumping
    unsigned int tfd_pix;
    unsigned int dump_vtog;
    unsigned int dump_length;

    unsigned int sweep_lines;
    unsigned int dump_lines;
    unsigned int dummy_read_lines;
    unsigned int vsg_lines;

    unsigned int dark_lines_u;
    unsigned int buf_lines_u;
    unsigned int active_lines;
    unsigned int buf_lines_d;
    unsigned int dark_lines_d;
    unsigned int lines;
    unsigned int field_lines;
} ccd_info_t;

typedef struct {
    bool     enable;
    float    downscale;
    uint8_t* vram_p;
} ccd2vram_config_t;

typedef struct {
    ccd_info_t*        ccdinfo_p;
    void*              exif_p;
    uint8_t*           ccd_buffer_p;
    ccd2vram_config_t* ccd2vram_p;
} effusus_db_t;



#endif /* _EFFUSUS_H_ */