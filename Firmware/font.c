//
// Project Bric
// Copyright 2020 Wenting Zhang
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at https://mozilla.org/MPL/2.0/.
//
#include <stdio.h>
#include "os_display.h"
#include "font.h"

#include "fonts/uni2oem2312.h"
#include "fonts/font_8_ascii.h"
#ifdef FNT_ENABLE_12
#include "fonts/font_12_ascii.h"
#include "fonts/font_12.h"
#endif
#ifdef FNT_ENABLE_16
#include "fonts/font_16_ascii.h"
#include "fonts/font_16.h"
#endif

static Font currentFont;
static uint8_t *fontASCII;
static uint8_t *fontGB;
static int fontHeight; // Height
static int fontWidth; // Half-width Width
static int fontSize; // Half-width size

void font_init() {
    font_set_font(FNT_8);
}

void font_set_font(Font font) {
    currentFont = font;
    if (currentFont == FNT_8) {
        fontASCII = (uint8_t *)font_8_ascii;
        fontGB = NULL;
        fontHeight = 8;
        fontWidth = 5;
        fontSize = 5;
    }
    else if (currentFont == FNT_12) {
#ifdef FNT_ENABLE_12
        fontASCII = (uint8_t *)font_12_ascii;
        fontGB = (uint8_t *)font_12;
#else
        fontASCII = NULL;
        fontGB = NULL;
#endif
        fontHeight = 12;
        fontWidth = 6;
        fontSize = 9;
    }
    else if (currentFont == FNT_16) {
#ifdef FNT_ENABLE_16
        fontASCII = (uint8_t *)font_16_ascii;
        fontGB = (uint8_t *)font_16;
#else
        fontASCII = NULL;
        fontGB = NULL;
#endif
        fontHeight = 16;
        fontWidth = 8;
        fontSize = 16;
    }
}

uint16_t font_ucs_to_gbk(uint16_t uni) {
    const uint16_t *p;
    uint16_t c = 0, uc;
    uint32_t i = 0, n, li, hi;

    if (uni < 0x80) {   /* ASCII? */
        c = (uint16_t)uni;

    } else { /* Non-ASCII */
        uc = (uint16_t)uni;
        p = uni2oem2312;
        hi = sizeof(uni2oem2312) / 4 - 1;
        li = 0;
        for (n = 16; n; n--) {
            i = li + (hi - li) / 2;
            if (uc == p[i * 2]) break;
            if (uc > p[i * 2]) {
                li = i;
            } else {
                hi = i;
            }
        }
        if (n != 0) c = p[i * 2 + 1];
    }

    return c;
}

// The graph is packed (byte not aligned with line)
void font_draw_graph(Canvas *canvas, int x, int y, uint32_t color, int w, int h, uint8_t *graph) {
    int xx = 0;
    int yy = 0;
    int bytes = w * h / 8;
    for (int i = 0; i < bytes; i++) {
        uint8_t byte = *graph++;
        for (int j = 0; j < 8; j++) {
            if (!(byte & 0x01)) {
                os_disp_set(canvas, x + xx, y + yy, color);
            }
            byte >>= 1;
            xx++;
            if (xx == w) {
                xx = 0;
                yy++;
            }
        }
    }
}

void font_draw_gb(Canvas *canvas, int x, int y, uint32_t color, uint32_t unicode, int *w, int *h) {
    uint16_t gbk = font_ucs_to_gbk(unicode);

    if (gbk < 0x80) {
        *w = 0;
        if (gbk < 0x20)
            return;
        // ASCII
        *w = fontWidth;
        *h = fontHeight;
        uint8_t *g;
        if (!fontASCII)
            return;
        g = fontASCII + (gbk - 0x20) * fontSize;
        font_draw_graph(canvas, x, y, color, fontWidth, fontHeight, g);
    }
    else {
        // look up GB2312 font
        *w = fontHeight; // Force square
        *h = fontHeight;
        if (!fontGB)
            return;
        uint8_t c1 = gbk & 0xff;
        uint8_t c2 = (gbk >> 8) & 0xff;
        if ((c1 < 0xa1) || (c2 < 0xa1))
            return;
        c1 -= 0xa1;
        if (c1 > 15) // Region 10-15 are empty, skip
            c1 -= 6;
        uint8_t *g = fontGB + (94 * c1 + (c2 - 0xa1)) * fontSize * 2;
        font_draw_graph(canvas, x, y, color, fontHeight, fontHeight, g);
    }
}

void font_draw(Canvas *canvas, int x, int y, uint32_t color, uint32_t unicode, int *w, int *h) {
    font_draw_gb(canvas, x, y, color, unicode, w, h);
}

uint32_t font_decode(char *c, CharEncoding encoding, int *size) {
    // todo: decode encoded char from string, return the UCS value, and set the size to the byte length of the encoded char
    uint32_t ucf = 0xfffd;
    uint8_t ch = *c++;
    switch (encoding) {
    case CE_ASCII:
        ucf = ch & 0xff;
        *size = 1;
        break;
    case CE_UCS2:
    	ucf = ch & 0xff;
    	ucf |= ((*c) << 8) & 0xff00;
    	*size = 2;
    	break;
    case CE_UTF8:
    	if ((ch & 0x80) == 0x00) {
    		ucf = ch & 0xff;
    		*size = 1;
    	}
    	else if ((ch & 0xe0) == 0xc0) {
    		ucf = (ch & 0x1f) << 6;
    		ucf |= (*c++ & 0x3f);
    		*size = 2;
    	}
    	else if ((ch & 0xf0) == 0xe0) {
    		ucf = (ch & 0x0f) << 12;
    		ucf |= (*c++ & 0x3f) << 6;
    		ucf |= (*c++ & 0x3f);
    		*size = 3;
    	}
    	else if ((ch & 0xf8) == 0xf0) {
			ucf = (ch & 0x0f) << 18;
			ucf |= (*c++ & 0x3f) << 12;
			ucf |= (*c++ & 0x3f) << 6;
			ucf |= (*c++ & 0x3f);
			*size = 4;
		}
    	else {
    		*size = 1;
    	}
    	break;
    case CE_UTF16:
    	ucf = ch & 0xff;
		ucf |= ((*c++) << 8) & 0xff00;
		if ((ucf & 0xfc00) == 0xd800) {
			uint32_t tmp = (*c++) & 0xff;
			tmp |= ((*c++) << 8) & 0xff00;
			ucf = ((ucf & 0x03ff) << 10) | (tmp & 0x03ff);
			*size = 4;
		}
		else {
			*size = 2;
		}
		break;
    default:
        fprintf(stderr, "Encoding not supported yet!\n");
        break;
    }
    return ucf;
}

void font_disp(Canvas *canvas, int x, int y, int width, uint32_t color,
        char *string, int len, CharEncoding encoding) {
    int count = 0, char_size = 0;
    int x0 = x;
    while ((count < len) && (string[count])) {
        uint32_t ucf = font_decode(&(string[count]), encoding, &char_size);
        count += char_size;
        if ((count == char_size) && (ucf == 0xfeff))
        	continue; // Skip BOM
        int w, h;
        if ((x > (x0 + width)) || (ucf == '\n')) {
            x = x0;
            y += h;
        }
        font_draw(canvas, x, y, color, ucf, &w, &h);
        x += w;
    }
}
