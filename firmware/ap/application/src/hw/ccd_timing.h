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
#pragma once

// Pixel time:
// 24MHz - 41.67ns
// 27MHz - 37ns (used on hardware)
// 30MHz - 33.33ns

// Timing for normal readout
#define CCD_TVCCD_PIX       94 // 3.5 us
#define CCD_THD_PIX         94 // 3.5 us
#define CCD_DUMMY_PIX       4
#define CCD_DARK_PIX        19
#define CCD_BUFFER_PIX      13
#define CCD_ACTIVE_PIX      2004
#define CCD_T3P_PIX         3240 // 120 us
#define CCD_TV3RD_PIX       270 // 10 us
#define CCD_T3D_PIX         540 // 20 us
#define CCD_LINE_PIXCNT     (CCD_DUMMY_PIX + CCD_DARK_PIX + CCD_BUFFER_PIX + CCD_ACTIVE_PIX)
#define CCD_HBLK_LENGTH     (CCD_TVCCD_PIX + CCD_THD_PIX)
#define CCD_LINE_LENGTH     (CCD_HBLK_LENGTH + CCD_LINE_PIXCNT)
#define CCD_VSG_LENGTH      (CCD_T3P_PIX + CCD_TV3RD_PIX + CCD_LINE_LENGTH)
#define CCD_CLPOB_BEGIN     (CCD_HBLK_LENGTH + CCD_DUMMY_PIX + 1)
#define CCD_CLPOB_END       (CCD_HBLK_LENGTH + CCD_DUMMY_PIX + CCD_DARK_PIX - 1)
#define AFE_TIM_OFFSET      36

// Timing for fast line dumping
#define CCD_TFD_PIX         47 // 1us
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


// Timing for preview readout
#define CCD_LINESKIPPING    11 // Skip 10 lines every 11 lines
// For 2720 lines, we run 2720/11=248 lines
// Each line is then 3888 pixels (spending lot of time on toggling V lines)
// Each frame is then 249 lines, totaling 249*3888=968112 cycles
// At 24MHz, this is 25FPS
// In draft mode, each line is 3888/24M=0.000162s, or about 1/6172s.
// 2 lines ~= 1/3000s
// 3 lines ~= 1/2000s
// 4 lines ~= 1/1500s
// 6 lines ~= 1/1000s
// 8 lines ~= 1/800s
// 15 lines ~= 1/400s
// 31 lines ~= 1/200s
// 62 lines ~= 1/100s
// 123 lines ~= 1/50s
// 247 lines ~= 1/25s <- not necessary, at 25FPS live view the natural exposure time is 1/25s
#define CCD_PRV_SKIP_LENGTH ((CCD_TVCCD_PIX + CCD_THD_PIX) * (CCD_LINESKIPPING - 1))
#define CCD_PRV_HBLK_LENGTH ((CCD_TVCCD_PIX + CCD_THD_PIX) * CCD_LINESKIPPING)
#define CCD_PRV_LINE_LENGTH (CCD_PRV_HBLK_LENGTH + CCD_LINE_PIXCNT)
#define CCD_PRV_CLPOB_BEGIN (CCD_PRV_HBLK_LENGTH + CCD_DUMMY_PIX + 1)
#define CCD_PRV_CLPOB_END   (CCD_PRV_HBLK_LENGTH + CCD_DUMMY_PIX + CCD_DARK_PIX - 1)

#define CCD_PRV_ARRAY_LINES (248)
#define CCD_PRV_FIELD_LINES (CCD_VSG_LINES + CCD_PRV_ARRAY_LINES)
