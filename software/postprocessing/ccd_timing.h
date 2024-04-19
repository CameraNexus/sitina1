//
// Project Sitina
// Copyright 2022 Wenting Zhang
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
// File : ccd_timing.h
// Brief: KAI-11000CM CCD Configuration

#pragma once

// Timing for normal readout
#define CCD_TVCCD_PIX       84 // 3.5 us
#define CCD_THD_PIX         84 // 3.5 us
#define CCD_DUMMY_PIX       4
#define CCD_DARK_PIX        19
#define CCD_BUFFER_PIX      13
#define CCD_ACTIVE_PIX      2004
#define CCD_T3P_PIX         2880 // 120 us
#define CCD_TV3RD_PIX       240 // 10 us
#define CCD_T3D_PIX         480 // 20 us
#define CCD_LINE_LENGTH     (CCD_TVCCD_PIX + CCD_THD_PIX + CCD_DUMMY_PIX + CCD_DARK_PIX + CCD_BUFFER_PIX + CCD_ACTIVE_PIX)
#define CCD_HBLK_LENGTH     (CCD_TVCCD_PIX + CCD_THD_PIX)
#define CCD_VSG_LENGTH      (CCD_T3P_PIX + CCD_TV3RD_PIX + CCD_LINE_LENGTH)
#define CCD_CLPOB_BEGIN     (CCD_HBLK_LENGTH + CCD_DUMMY_PIX + 1)
#define CCD_CLPOB_END       (CCD_HBLK_LENGTH + CCD_DUMMY_PIX + CCD_DARK_PIX - 1)

// Timing for fast line dumping
#define CCD_TFD_PIX         24 // 1us
#define CCD_DUMP_VTOG       (CCD_TFD_PIX + CCD_TVCCD_PIX)
#define CCD_DUMP_LENGTH     (CCD_TVCCD_PIX * 2)

#define CCD_SWEEP_LINES     (207) // For timing purposes, in complete line timing
#define CCD_DUMP_LINES      (2720) // Lines need to be dumped
#define CCD_DUMMY_READ_LINES (2)
#define CCD_VSG_LINES       (1)

#define CCD_DARK_LINES_U    (16)
#define CCD_BUF_LINES_U     (8)
#define CCD_ACTIVE_LINES    (2672)
#define CCD_BUF_LINES_D     (8)
#define CCD_DARK_LINES_D    (17)
#define CCD_LINES           (CCD_DARK_LINES_U + CCD_BUF_LINES_U + CCD_ACTIVE_LINES + CCD_BUF_LINES_D + CCD_DARK_LINES_D)
//#define CCD_FIELD_LINES     (CCD_SWEEP_LINES + CCD_DUMMY_READ_LINES + CCD_VSG_LINES + CCD_LINES)
#define CCD_FIELD_LINES     (CCD_DUMMY_READ_LINES + CCD_VSG_LINES + CCD_LINES)
