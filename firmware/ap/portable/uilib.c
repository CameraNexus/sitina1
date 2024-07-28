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
#include <stdio.h> // For debug printing messages
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include "uilib.h"

// Private definitions
#define MIN(a,b)    ((a) < (b) ? (a) : (b))
#define MAX(a,b)    ((a) > (b) ? (a) : (b))
#define MAX_NUM_FB  (3)

// Private types
typedef struct {
    uint8_t *addr;
    uint32_t counter;   // Content ID, force redraw when 
} fb_info;

// Private variables
static uint8_t *current_framebuffer;
static const UIFONT *current_font;
static uint32_t current_font_byteperchar;
static uint32_t current_content_counter;
static fb_info framebuffer_info[MAX_NUM_FB];
static fb_info *current_framebuffer_info;

// Private functions

// Putpixel only writes to framebuffer, fixed format
static void uilib_putpixel(uint32_t x, uint32_t y, uint32_t cl) {
#if defined(UILIB_FB_XRGB8888)
    uint32_t *fb = (uint32_t *)current_framebuffer;
    fb[y * UILIB_FB_WIDTH + x] = cl;
#elif defined(UILIB_FB_RGB565)
    uint16_t *fb = (uint16_t *)current_framebuffer;
    fb[y * UILIB_FB_WIDTH + x] = cl;
#elif defined(UILIB_FB_Y1)
    uint8_t *fb = (uint8_t *)current_framebuffer + (y * UILIB_FB_WIDTH / 8 + x / 8);
    if (cl)
        *fb |= 1 << (x % 8);
    else
        *fb &= ~(1 << (x % 8));
#endif
}

// Getpixel can get from any buffer, selectable format, return in FB format
static uint32_t uilib_getpixel(const uint8_t *fb, UIPIXFMT pixfmt, uint32_t w,
        uint32_t h, uint32_t x, uint32_t y) {
    uint32_t rgb;
    uint16_t *fb16 = (uint16_t *)fb;
    uint32_t *fb32 = (uint32_t *)fb;
    switch (pixfmt) {
    case PIXFMT_Y1:
        rgb = (fb[y * w / 8 + x / 8] >> (x % 8)) & 0x1;
        rgb = rgb ? 0xFFFFFF : 0x0;
        break;
    case PIXFMT_Y2:
        rgb = (fb[y * w / 4 + x / 4] >> (x % 4)) & 0x3;
        rgb |= rgb << 2;
        rgb |= rgb << 4;
        rgb |= (rgb << 8) | (rgb << 16);
        break;
    case PIXFMT_Y4:
        rgb = (fb[y * w / 2 + x / 2] >> (x % 2)) & 0xF;
        rgb |= rgb << 4;
        rgb |= (rgb << 8) | (rgb << 16);
        break;
    case PIXFMT_Y8:
        rgb = fb[y * w + x];
        rgb |= (rgb << 8) | (rgb << 16);
        break;
    case PIXFMT_RGB565:
        rgb = fb16[y * w + x];
        rgb = (RGB565_GET_R(rgb) << 16) | (RGB565_GET_G(rgb) << 8) |
            RGB565_GET_B(rgb);
        break;
    case PIXFMT_XRGB8888:
        rgb = fb32[y * w + x];
        break;
    }
    return COLOR_RGB2FB(rgb);
}

static uint32_t uilib_get_char_width(uint32_t chr) {
    switch (current_font->type) {
    case FONTTYPE_MONOSPACE_ASCII:
        return current_font->disp_width;
        break;
    // TODO: other fonts
    }
    return 0;
}

static uint32_t uilib_blend(uint32_t fg, uint32_t bg, uint8_t alpha) {
    uint32_t fg_r = COLOR_FB2R(fg);
    uint32_t fg_g = COLOR_FB2G(fg);
    uint32_t fg_b = COLOR_FB2B(fg);
    uint32_t bg_r = COLOR_FB2R(bg);
    uint32_t bg_g = COLOR_FB2G(bg);
    uint32_t bg_b = COLOR_FB2B(bg);
    uint32_t r = (fg_r * alpha + bg_r * (255 - alpha)) / 255;
    uint32_t g = (fg_g * alpha + bg_g * (255 - alpha)) / 255;
    uint32_t b = (fg_b * alpha + bg_b * (255 - alpha)) / 255;
    uint32_t rgb = (r << 16) | (g << 8) | b;
    return COLOR_RGB2FB(rgb);
}

static uint32_t uilib_draw_char(uint32_t x, uint32_t y, uint32_t chr,
        bool transparent, uint32_t fgcl, uint32_t bgcl) {
    // Non anti-aliased font:
    //  - Transparent: only draw pixel that's ON with fgcl
    //  - Non-transparent: draw pixel that's ON with fgcl and OFF with bgcl
    // Anti-aliased font:
    //  - Transparent: alpha blend with underlying pixel
    //  - Non-transparent: alpha blend with bgcl
    uint32_t bw = current_font->buf_width;
    uint32_t dw = current_font->disp_width;
    uint32_t h = current_font->height;
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
        break;
    }
    if (current_font->pixfmt == PIXFMT_Y1) {
        for (uint32_t yy = 0; yy < h; yy++) {
            uint8_t byte;
            for (uint32_t xx = 0; xx < dw; xx++) {
                if (xx % 8 == 0) {
                    byte = gbuf[yy * bw / 8 + xx / 8];
                }
                else {
                    byte >>= 1;
                }
                if (byte & 0x1)
                    uilib_putpixel(x + xx, y + yy, fgcl);
                else if (!transparent) {
                    uilib_putpixel(x + xx, y + yy, bgcl);
                }
            }
        }
    }
    else {
        for (uint32_t yy = 0; yy < h; yy++) {
            for (uint32_t xx = 0; xx < dw; xx++) {
                uint32_t a =
                    uilib_getpixel(gbuf, current_font->pixfmt, bw, h, xx, yy);
                uint32_t bg_blending = transparent ?
                    uilib_getpixel(current_framebuffer, UILIB_FB_FORMAT,
                        UILIB_FB_WIDTH, UILIB_FB_HEIGHT, xx, yy) : bgcl;
                uint32_t cl = uilib_blend(fgcl, bg_blending, a);
                uilib_putpixel(x + xx, y + yy, cl);
            }
        }
    }

    return dw;
}

static uint32_t uilib_get_string_width(char *string, uint32_t maxlen) {
    uint32_t len = 0;
    char c;
    uint32_t i = 0;
    while ((i++ < maxlen) && (c = *string++)) {
        len += uilib_get_char_width(c);
    }
    return len;
}

static void uilib_draw_label(UICOMP *label) {
    if (!label->specifics.label.transparent) {
        uilib_fill_rect(label->x, label->y, label->w, label->h,
            label->specifics.label.bgcl);
    }
    uilib_set_font(label->specifics.label.font);
    if (label->specifics.label.align == ALIGN_LEFT) {
        uilib_draw_string(label->x, label->y, label->specifics.label.string,
            UILIB_LABEL_MAXLEN, label->specifics.label.transparent,
            label->specifics.label.fgcl, label->specifics.label.bgcl);
    }
    else {
        uint32_t width = uilib_get_string_width(label->specifics.label.string,
            UILIB_LABEL_MAXLEN);
        uint32_t x = (label->specifics.label.align == ALIGN_RIGHT) ?
            label->x + label->w - width : label->x + (label->w - width) / 2;
        uilib_draw_string(x, label->y, label->specifics.label.string,
            UILIB_LABEL_MAXLEN, label->specifics.label.transparent,
            label->specifics.label.fgcl, label->specifics.label.bgcl);
    }
}

static void uilib_draw_bitmap(UICOMP *bitmap) {
    // Determine the draw size
    uint32_t w = MIN(bitmap->w, bitmap->specifics.bitmap.buf_width);
    uint32_t h = MIN(bitmap->h, bitmap->specifics.bitmap.buf_height);
    if (((UILIB_FB_FORMAT == PIXFMT_XRGB8888) ||
        (UILIB_FB_FORMAT == PIXFMT_RGB565) || (UILIB_FB_FORMAT == PIXFMT_Y8)) &&
        (bitmap->specifics.bitmap.pixelformat == UILIB_FB_FORMAT)) {
        // Can directly copy
    #if defined(UILIB_FB_XRGB8888)
        uint32_t *srcptr = (uint32_t *)(bitmap->specifics.bitmap.pbuf);
        uint32_t *dstptr = (uint32_t *)current_framebuffer;
    #elif defined(UILIB_FB_RGB565)
        uint16_t *srcptr = (uint16_t *)(bitmap->specifics.bitmap.pbuf);
        uint16_t *dstptr = (uint16_t *)current_framebuffer;
    #elif defined(UILIB_FB_Y8)
        uint8_t *srcptr = (uint8_t *)(bitmap->specifics.bitmap.pbuf);
        uint8_t *dstptr = (uint8_t *)current_framebuffer;
    #endif
        dstptr += bitmap->y * UILIB_FB_WIDTH + bitmap->x;
        for (int y = 0; y < h; y++) {
            memcpy(dstptr, srcptr, w * UILIB_FB_BYTEPERPIX);
            srcptr += bitmap->specifics.bitmap.buf_width;
            dstptr += UILIB_FB_WIDTH;
        }
    }
    else {
        // Slow path
        for (int y = 0; y < h; y++) {
            for (int x = 0; x < w; x++) {
                uint32_t p = uilib_getpixel(
                    bitmap->specifics.bitmap.pbuf,
                    bitmap->specifics.bitmap.pixelformat,
                    bitmap->specifics.bitmap.buf_width,
                    bitmap->specifics.bitmap.buf_height,
                    x, y);
                uilib_putpixel(bitmap->x + x, bitmap->y + y, p);
            }
        }
    }
}

static void uilib_draw_rect(UICOMP *rect) {
    uilib_fill_rect(rect->x, rect->y, rect->w, rect->h,
            rect->specifics.rect.cl);
}

static void uilib_draw_component(UICOMP *comp) {
    switch (comp->type) {
    case COMP_LABEL:
        uilib_draw_label(comp);
        break;
    case COMP_BITMAP:
        uilib_draw_bitmap(comp);
        break;
    case COMP_RECT:
        uilib_draw_rect(comp);
        break;
    case COMP_CUSTOM:
        comp->specifics.custom.drawfunc((void *)comp);
        break;
    }
}

static uint32_t uilib_get_bufsize(UIPIXFMT pixfmt, uint32_t w, uint32_t h) {
    uint32_t size = w * h;
    switch (pixfmt)
    {
    case PIXFMT_Y1: size /= 8; break;
    case PIXFMT_Y2: size /= 4; break;
    case PIXFMT_Y4: size /= 2; break;
    case PIXFMT_RGB565: size *= 2; break;
    case PIXFMT_XRGB8888: size *= 4; break;
    default: break;
    }
    return size;
}

// Public functions
void uilib_init(void) {
    current_content_counter = 0;
    for (int i = 0; i < MAX_NUM_FB; i++) {
        framebuffer_info[i].addr = NULL;
    }
}

void uilib_set_framebuffer(uint8_t *fb) {
    current_framebuffer = fb;
    current_framebuffer_info = NULL;
    for (int i = 0; i < MAX_NUM_FB; i++) {
        if (framebuffer_info[i].addr == fb) {
            current_framebuffer_info = &framebuffer_info[i];
            break;
        }
        else if (framebuffer_info[i].addr == NULL) {
            printf("Registering framebuffer %p\n", fb);
            framebuffer_info[i].addr = fb;
            current_framebuffer_info = &framebuffer_info[i];
            break;
        }
    }
    assert(current_framebuffer_info);
}

void uilib_fill_rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t cl) {
#if defined(UILIB_FB_XRGB8888)
    uint32_t *wptr = (uint32_t *)current_framebuffer;
    wptr += y * UILIB_FB_WIDTH + x;
    for (uint32_t yy = y; yy < (y + h); yy++) {
        for (uint32_t xx = 0; xx < w; xx++) {
            *wptr++ = cl;
        }
        wptr += (UILIB_FB_WIDTH - w); // Move to next line
    }
#elif defined(UILIB_FB_RGB565)
    uint16_t *wptr = (uint16_t *)current_framebuffer;
    wptr += y * UILIB_FB_WIDTH + x;
    for (uint32_t yy = y; yy < (y + h); yy++) {
        memset(wptr, 0, w * 2);
        wptr += UILIB_FB_WIDTH;
    }
#else
    for (uint32_t yy = y; yy < (y + h); yy++) {
        for (uint32_t xx = x; xx < (x + w); xx++) {
            uilib_putpixel(xx, yy, cl);
        }
    }
#endif
}

void uilib_set_font(const UIFONT *font) {
    current_font = font;
    current_font_byteperchar =
        uilib_get_bufsize(font->pixfmt, font->buf_width, font->height);
}

void uilib_draw_string(uint32_t x, uint32_t y, char *string, uint32_t maxlen,
        bool transparent, uint32_t fgcl, uint32_t bgcl) {
    // TODO: Unicode/ CJK
    char c;
    uint32_t i = 0;
    while ((i++ < maxlen) && (c = *string++)) {
        x += uilib_draw_char(x, y, c, transparent, fgcl, bgcl);
    }
}

void uilib_mark_update(void) {
    current_content_counter ++;
}

void uilib_draw(UIDRAWLIST *drawlist) {
    // Check if draw is necessary
    if (current_content_counter == current_framebuffer_info->counter)
        return;

    printf("Redraw for framebuffer %p\n", current_framebuffer);

    memset(current_framebuffer, 0,
        uilib_get_bufsize(UILIB_FB_FORMAT, UILIB_FB_WIDTH, UILIB_FB_HEIGHT));
    for (int i = 0; i < drawlist->ncomp; i++) {
        uilib_draw_component(drawlist->comp[i]);
    }

    // Mark framebuffer as up to date
    current_framebuffer_info->counter = current_content_counter;
}
