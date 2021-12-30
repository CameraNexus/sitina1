//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
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

// Horizontal UI when the screen is vertical
#define ROTATE_UI

#ifndef LARGE_UI
#ifdef ROTATE_UI
#define UI_HEIGHT (FB_WIDTH)
#define UI_WIDTH (FB_HEIGHT)
#else
#define UI_HEIGHT FB_HEIGHT
#define UI_WIDTH FB_WIDTH
#endif
#endif

void ui_init(void);
void ui_clear(uint16_t c);
void ui_disp_string(int x, int y, char *str, uint16_t c);
int ui_disp_string_bb(char *str, int width);
//void ui_disp_fill(int x1, int y1, int x2, int y2, uint16_t c);
void ui_disp_num(int x, int y, uint32_t num, uint16_t c);
void ui_disp_hex(int x, int y, uint32_t num, uint16_t c);
int ui_printf(int x, int y, const char *format, ...);
