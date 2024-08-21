//
// Sitina1
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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "uilib.h"
#include "pal_display.h"

static const UIFONT *current_font;
static uint8_t fb[AOND_WIDTH * AOND_HEIGHT / 8];

void aondisp_clear(uint8_t cl) {
    cl = cl ? 0xFF : 0x00;
    memset(fb, cl, sizeof(fb));
}

void aondisp_update(void) {
    pal_disp_update_aondisp(fb);
}

void aondisp_set_font(const UIFONT *font) {
    current_font = font;
}

static void aondisp_putpixel(uint32_t x, uint32_t y, uint32_t cl) {
    uint8_t *ptr = &fb[y * AOND_WIDTH / 8 + x / 8];
    if (cl)
        *ptr |= 1 << (x % 8);
    else
        *ptr &= ~(1 << (x % 8));
}

static uint32_t aondisp_draw_char(uint32_t x, uint32_t y, uint32_t chr,
        uint32_t cl) {
    uint32_t bw = current_font->buf_width;
    uint32_t dw = current_font->disp_width;
    uint32_t h = current_font->height;
    uint32_t current_font_byteperchar = bw * h / 8;
    const uint8_t *gbuf;
    switch  (current_font->type) {
    case FONTTYPE_MONOSPACE_ASCII:
        chr = chr - current_font->offset;
        if (chr > current_font->chars)
            return 0;
        gbuf = current_font->font + chr * current_font_byteperchar;
        break;
    default:
        // TODO
        return 0;
        break;
    }
    if (current_font->pixfmt != PIXFMT_Y1)
        return;
    for (uint32_t yy = 0; yy < h; yy++) {
        uint8_t byte = 0; // not necessary, to silence warning
        for (uint32_t xx = 0; xx < dw; xx++) {
            if (xx % 8 == 0) {
                byte = gbuf[yy * bw / 8 + xx / 8];
            }
            else {
                byte >>= 1;
            }
            if (byte & 0x1)
                aondisp_putpixel(x + xx, y + yy, cl);
        }
    }
    return dw;
}

void aondisp_draw_string(uint32_t x, uint32_t y, char *string, uint32_t maxlen,
        uint32_t cl) {
    char c;
    uint32_t i = 0;
    while ((i++ < maxlen) && (c = *string++)) {
        x += aondisp_draw_char(x, y, c, cl);
    }
}
