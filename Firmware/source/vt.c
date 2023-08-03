//
// SitinaNe
// Copyright 2023 Wenting Zhang
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
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include "vout.h"
#include "font.h"

#define COLOR_BG 0x0000
#define COLOR_FG 0xffff

#define SCR_WIDTH (FB_WIDTH / 8)
#define SCR_HEIGHT (FB_HEIGHT / 16)

static uint8_t screenbuf[SCR_HEIGHT][SCR_WIDTH];
static int x, y;

static void vt_set_pixel(size_t x, size_t y, uint16_t c) {
    uint16_t *fbptr = framebuffer;
    fbptr[y * FB_WIDTH + x] = c;
}

static void vt_disp_char(int x, int y, uint8_t c) {
    for (int yy = 0; yy < 12; yy++) {
        for (int xx = 0; xx < 8; xx++) {
            if ((charMap_ascii[c][yy] >> xx) & 0x01) {
                vt_set_pixel(x + xx, y + yy, COLOR_FG);
            }
            else {
                vt_set_pixel(x + xx, y + yy, COLOR_BG);
            }
        }
    }
}

void vt_init(void) {
    memset(screenbuf, 0x20, sizeof(screenbuf));
    x = 0;
    y = 0;
}

static void vt_scroll() {
    memcpy(screenbuf, screenbuf + SCR_WIDTH, (SCR_HEIGHT - 1) * (SCR_WIDTH));
    // Redraw everything
    for (int y = 0; y < SCR_HEIGHT; y++) {
        for (int x = 0; x < SCR_WIDTH; x++) {
            vt_disp_char(x * 8, y * 16, screenbuf[y][x]);
        }
    }
}

void vt_putc(uint8_t c) {
    if (c == '\n') {
        x = 0;
        y += 1;
        if (y == SCR_HEIGHT) {
            vt_scroll();
            y--;
        }
    }
    else if (c == '\r') {
        x = 0;
    }
    else {
        x++;
        if (x == SCR_WIDTH) {
            x = 0;
            y += 1;
            if (y == SCR_HEIGHT) {
                vt_scroll();
                y--;
            }
        }
        screenbuf[y][x] = c;
        vt_disp_char(x * 8, y * 16, c);
    }
}

__attribute__((used)) int _write(int fd, char *buf, int size)
{
    int i;

    for(i = 0; i < size; i++)
    {
		vt_putc((uint8_t)buf[i]);
    }

    return size;
}
