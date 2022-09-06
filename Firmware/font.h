//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#ifndef __FONT_H__
#define __FONT_H__

//#define FNT_ENABLE_8
//#define FNT_ENABLE_12
#define FNT_ENABLE_16

typedef enum {
    CE_UCS2,
    CE_UTF8,
	CE_UTF16,
    CE_ASCII,
    CE_SHIFT_JIS,
    CE_GB2312
} CharEncoding;

typedef enum {
    FNT_8, // 5x8 font
    FNT_12, // 12x12 font
    FNT_16, // 16x16 font
} Font;

void font_init();
void font_set_font(Font font);
void font_draw(Canvas *canvas, int x, int y, uint32_t color, uint32_t unicode,
        int *w, int *h);
uint32_t font_decode(char *c, CharEncoding encoding, int *size);
void font_disp(Canvas *canvas, int x, int y, int width, uint32_t color,
        char *string, int len, CharEncoding encoding);

#endif
