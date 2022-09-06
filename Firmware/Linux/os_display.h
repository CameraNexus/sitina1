//
// Project Fobu
// Copyright 2020 Wenting Zhang
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
// File : os_display.h
// Brief: Display Abstraction Layer.
//
#pragma once

#include <stdint.h>

#define DISP_WIDTH (640)
#define DISP_HEIGHT (480)

typedef enum pixel_format {
    PIXFMT_Y1,
    PIXFMT_Y2,
    PIXFMT_Y4,
    PIXFMT_Y8, 
    PIXFMT_RGB565,
    PIXFMT_RGB888,
    PIXFMT_ARGB8888,
    PIXFMT_RGBA8888
} PixelFormat;

typedef struct canvas {
    int width;
    int height;
    PixelFormat pixelFormat;
    char buf[];
} Canvas;

// Power control
void os_disp_init();
void os_disp_deinit();

void os_disp_enter_sleep();
void os_disp_exit_sleep();

// Framebuffer operation
Canvas *os_disp_create(int w, int h, PixelFormat fmt);
void os_disp_set(Canvas *dst, int x, int y, uint32_t color);
uint32_t os_disp_get(Canvas *src, int x, int y);
void os_disp_fill(Canvas *dst, int x, int y, int w, int h, uint32_t color);
void os_disp_bilt(
        Canvas *dst, Canvas *src,
        int dst_x, int dst_y, int dst_w, int dst_h,
        int src_x, int src_y, int src_w, int src_h);
void os_disp_bilt_raw(
        Canvas *dst, uint8_t *src,
        int dst_x, int dst_y,
        int src_x, int src_y, int src_w, int src_h);
void os_disp_conv(Canvas *dst, Canvas *src);

// Commit to screen
void os_disp_draw(Canvas *src);
