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
#pragma once

#include <stdint.h>
#include <stdbool.h>

// Framebuffer format
// Define only one of these
#define UILIB_FB_XRGB8888

// Framebuffer SIZE
#define UILIB_FB_WIDTH      (480)
#define UILIB_FB_HEIGHT     (480)

// Various configurations
#define UILIB_LABEL_MAXLEN  (32)

#define RGB_TO_XRGB8888(x)  (x)
#define RGB_TO_RGB565(x)    (((x & 0xff) >> 3) | \
                            ((((x >> 8) & 0xff) >> 2) << 5) | \
                            ((((x >> 16) & 0xff) >> 3) << 11))
#define RGB_TO_Y8(x)        ((((x >> 16) & 0xff) + ((x >> 8) & 0xff) + \
                            (x & 0xff)) / 3)
#define RGB_TO_Y4(x)        (RGB_TO_Y8(x) >> 4)
#define RGB_TO_Y2(x)        (RGB_TO_Y8(x) >> 6)
#define RGB_TO_Y1(x)        ((((x & 0x80) >> 7) + ((x & 0x8000) >> 15) + \
                            ((x & 0x800000) >> 23)) >> 1)

#define XRGB8888_GET_R(x)   ((x >> 16) & 0xff)
#define XRGB8888_GET_G(x)   ((x >> 8) & 0xff)
#define XRGB8888_GET_B(x)   (x & 0xff)

#define RGB565_GET_R(x)     (((x & 0xf800) >> 8) | (x >> 13))
#define RGB565_GET_G(x)     (((x & 0x07e0) >> 3) | ((x & 0x07e0) >> 9))
#define RGB565_GET_B(x)     (((x & 0x001f) << 3) | ((x & 0x001f) >> 2))

#define Y8_GET_Y(x)         (x & 0xff)
#define Y4_GET_Y(x)         (x | (x << 4))
#define Y2_GET_Y(x)         (x | (x << 2) | (x << 4) | (x << 6))
#define Y1_GET_Y(x)         (x ? 0xff : 0x00)

// FB format dependent definitions, no need to modify
#if defined(UILIB_FB_XRGB8888)
#define UILIB_FB_FORMAT     (PIXFMT_XRGB8888)
#define UILIB_FB_BYTEPERPIX (4)
#define COLOR_RGB2FB(x)     RGB_TO_XRGB8888(x)
#define COLOR_FB2R(x)       XRGB8888_GET_R(x)
#define COLOR_FB2G(x)       XRGB8888_GET_G(x)
#define COLOR_FB2B(x)       XRGB8888_GET_B(x)
#elif defined(UILIB_FB_RGB565)
#define UILIB_FB_FORMAT     (PIXFMT_RGB565)
#define UILIB_FB_BYTEPERPIX (2)
#define COLOR_RGB2FB(x)     RGB_TO_RGB565(x)
#define COLOR_FB2R(x)       RGB565_GET_R(x)
#define COLOR_FB2G(x)       RGB565_GET_G(x)
#define COLOR_FB2B(x)       RGB565_GET_B(x)
#elif defined(UILIB_FB_Y1)
#define UILIB_FB_FORMAT     (PIXFMT_Y1)
#define COLOR_RGB2FB(x)     RGB_TO_Y1(x)
#define COLOR_FB2R(x)       Y1_GET_Y(x)
#define COLOR_FB2G(x)       Y1_GET_Y(x)
#define COLOR_FB2B(x)       Y1_GET_Y(x)
#error "Unknown color depth"
#endif



#define COLOR_BLACK         COLOR_RGB2FB(0x000000)
#define COLOR_RED           COLOR_RGB2FB(0xFF0000)
#define COLOR_YELLOW        COLOR_RGB2FB(0xFFFF00)
#define COLOR_GREEN         COLOR_RGB2FB(0x00FF00)
#define COLOR_CYAN          COLOR_RGB2FB(0x00FFFF)
#define COLOR_BLUE          COLOR_RGB2FB(0x0000FF)
#define COLOR_MAGENTA       COLOR_RGB2FB(0xFF00FF)
#define COLOR_WHITE         COLOR_RGB2FB(0xFFFFFF)
#define COLOR_GREY39        COLOR_RGB2FB(0x646464)
#define COLOR_GREY41        COLOR_RGB2FB(0x696969)
#define COLOR_GREY63        COLOR_RGB2FB(0xA0A0A0)
#define COLOR_GREY89        COLOR_RGB2FB(0xE3E3E3)
#define COLOR_GREY94        COLOR_RGB2FB(0xF0F0F0)

typedef enum {
    COMP_LABEL,
    COMP_BITMAP,
    COMP_RECT,
    COMP_CUSTOM
} UICOMPTYPE;

typedef enum {
    PIXFMT_Y1,
    PIXFMT_Y2,
    PIXFMT_Y4,
    PIXFMT_Y8,
    PIXFMT_RGB565,
    PIXFMT_XRGB8888
} UIPIXFMT;

typedef enum {
    FONTTYPE_MONOSPACE_ASCII,
    FONTTYPE_PROPORTIONAL_ASCII,
    FONTTYPE_GBK
} UIFONTTYPE;

typedef enum {
    ALIGN_LEFT,
    ALIGN_RIGHT,
    ALIGN_CENTER
} UIALIGN;

typedef struct {
    UIFONTTYPE type;
    uint32_t buf_width;
    uint32_t disp_width;
    uint32_t height;
    uint32_t offset;
    uint32_t chars;
    UIPIXFMT pixfmt;
    uint8_t font[];
} UIFONT;

typedef struct {
    bool transparent;
    uint32_t fgcl;
    uint32_t bgcl;
    const UIFONT *font;
    UIALIGN align;
    char string[UILIB_LABEL_MAXLEN];
} UICOMP_LABEL;

typedef struct {
    UIPIXFMT pixelformat;
    uint32_t buf_width;
    uint32_t buf_height;
    uint8_t *pbuf;
} UICOMP_BITMAP;

typedef struct {
    uint32_t cl;
} UICOMP_RECT;

typedef void (*UICOMP_DRAWFUNC)(void *comp);

typedef struct {
    void *user; // Can store pointer or any other things fits
    UICOMP_DRAWFUNC drawfunc;
} UICOMP_CUSTOM;

typedef struct {
    UICOMPTYPE type;
    uint32_t x;
    uint32_t y;
    uint32_t w;
    uint32_t h;
    union {
        UICOMP_LABEL label;
        UICOMP_BITMAP bitmap;
        UICOMP_RECT rect;
        UICOMP_CUSTOM custom;
    } specifics;
} UICOMP;

typedef struct {
    int ncomp;
    UICOMP *comp[];
} UIDRAWLIST;

extern const UIFONT font_8x14;
extern const UIFONT font_12x20;
extern const UIFONT font_16x26;
extern const UIFONT font_24x40;
extern const UIFONT font_32x53;

void uilib_init(void);
void uilib_set_framebuffer(uint8_t *fb);
void uilib_set_font(const UIFONT *font);
// LL functions for custom component only or when update/redraw features are not used
void uilib_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cl);
void uilib_draw_string(uint32_t x, uint32_t y, char *string, uint32_t maxlen,
    bool transparent, uint32_t fgcl, uint32_t bgcl);
void uilib_mark_update(void); // Called before redraw, when things are expected to change
void uilib_draw(UIDRAWLIST *drawlist); // Called every frame
