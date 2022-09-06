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
// File : os_display.c
// Brief: Display Abstraction Layer.
//
#include <SDL.h>
#include "os_display.h"

static SDL_Surface *screen;

// Power control
void os_disp_init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        exit(1);
    }
    screen = SDL_SetVideoMode(DISP_WIDTH, DISP_HEIGHT, 16, SDL_SWSURFACE);
    if (screen == NULL) {
        exit(1);
    }
}

void os_disp_deinit() {
    SDL_Quit();
}

void os_disp_enter_sleep() {
    printf("os_disp: enter sleep\n");
}

void os_disp_exit_sleep() {
    printf("os_disp: exit sleep\n");
}

static int os_disp_get_bpp(PixelFormat fmt) {
    switch(fmt) {
    case PIXFMT_Y1:
        return 1;
    case PIXFMT_Y2:
        return 2;
    case PIXFMT_Y4:
        return 4;
    case PIXFMT_Y8:
        return 8;
    case PIXFMT_RGB565:
        return 16;
    case PIXFMT_RGB888:
        return 24;
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        return 32;
    }
    return 0; // Unknown
}

static uint8_t os_disp_get_mask(PixelFormat fmt) {
    switch(fmt) {
    case PIXFMT_Y1:
        return 0x01;
    case PIXFMT_Y2:
        return 0x03;
    case PIXFMT_Y4:
        return 0x0f;
    default:
        return 0x00;
    }
    return 0x00; // Unnecessary, but gcc gives a warning if I don't do so
}

// Framebuffer operation
Canvas *os_disp_create(int w, int h, PixelFormat fmt) {
    size_t size = w * h;
    int bpp = os_disp_get_bpp(fmt);
    if (bpp >= 8) {
        size *= (bpp / 8);
    }
    else {
        size /= (8 / bpp);
    }
    Canvas *canvas = malloc(sizeof(Canvas) + size);
    canvas->width = w;
    canvas->height = h;
    canvas->pixelFormat = fmt;
    return canvas;
}

void os_disp_set(
        Canvas *dst, int x, int y, uint32_t color) {
    int bpp;
    int pbb;
    int sft;
    uint8_t mask;
    size_t offset;

    if (x >= dst->width)
        return;
    if (y >= dst->height)
        return;
    if (x < 0)
        return;
    if (y < 0)
        return;

    switch(dst->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = os_disp_get_bpp(dst->pixelFormat);
        pbb = 8 / bpp;
        sft = x % pbb;
        mask = os_disp_get_mask(dst->pixelFormat);
        offset = y * dst->width / pbb + x / pbb;
        dst->buf[offset] &= ~(mask << sft);
        dst->buf[offset] |= (uint8_t)color << sft;
        break;
    case PIXFMT_Y8:
        offset = y * dst->width + x;
        dst->buf[offset] = (uint8_t)color;
        break;
    case PIXFMT_RGB565:
        offset = (y * dst->width + x) * 2;
        *(uint16_t *)(&(dst->buf[offset])) = (uint16_t)color;
        break;
    case PIXFMT_RGB888:
        offset = (y * dst->width + x) * 3;
        dst->buf[offset] = (uint8_t)(color & 0xff);
        dst->buf[offset+1] = (uint8_t)((color >> 16) & 0xff);
        dst->buf[offset+2] = (uint8_t)((color >> 24) & 0xff);
        break;
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        offset = (y * dst->width + x) * 4;
        *(uint32_t *)(&(dst->buf[offset])) = color;
        break;
    }
}

uint8_t os_disp_pix_to_y(uint32_t color, PixelFormat fmt) {
    uint8_t y = (uint32_t)color;
    switch (fmt) {
    case PIXFMT_Y1:
        y = (color) ? 0xff : 0x00;
        break;
    case PIXFMT_Y2:
        y |= y << 2; __attribute__ ((fallthrough));
    case PIXFMT_Y4:
        y |= y << 4; __attribute__ ((fallthrough));
    case PIXFMT_Y8:
        break;
    default:
        fprintf(stderr, "Converting from color to Y\n");
        break;
    }
    return y;
}

uint32_t os_disp_get(Canvas *src, int x, int y) {
    int bpp;
    int pbb;
    int sft;
    uint8_t mask;
    size_t offset;
    uint32_t color;

    switch(src->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = os_disp_get_bpp(src->pixelFormat);
        pbb = 8 / bpp;
        sft = x % pbb;
        mask = os_disp_get_mask(src->pixelFormat);
        offset = y * src->width / pbb + x / pbb;
        color = src->buf[offset] >> sft;
        return (uint32_t)(color & mask);
        break;
    case PIXFMT_Y8:
        offset = y * src->width + x;
        return (uint32_t)src->buf[offset];
        break;
    case PIXFMT_RGB565:
        offset = y * src->width + x;
        return ((uint16_t *)src->buf)[offset];
        break;
    case PIXFMT_RGB888:
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
    default:
        fprintf(stderr, "Unsupported pixel format\n");
        break;
    }
    return 0;
}

void os_disp_fill(Canvas *dst, int x, int y, int w, int h, uint32_t color) {
    int bpp;
    int pbb;
    size_t offset;
    uint16_t *ptr16;

    switch(dst->pixelFormat) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
        bpp = os_disp_get_bpp(dst->pixelFormat);
        pbb = 8 / bpp;
        if ((x % pbb == 0) && (w % pbb == 0)) {
            // Aligned, do byte operation directly
            uint8_t colorfill = os_disp_pix_to_y(color, dst->pixelFormat);
            for (int yy = y; yy < y + h; yy++) {
                offset = (yy * dst->width + x) / pbb;
                for (int xx = 0; xx < (w / pbb); xx++)
                    dst->buf[offset++] = colorfill;
            }
        }
        else {
            // Not aligned, fallback to using setpixel
            for (int yy = y; yy < y + h; yy++) {
                for (int xx = x; xx < x + w; xx++)
                    os_disp_set(dst, x, y, color);
            }
        }
        break;
    case PIXFMT_Y8:
        for (int yy = y; yy < y + h; yy++) {
            offset = yy * dst->width + x;
            for (int xx = 0; xx < w; xx++)
                dst->buf[offset++] = (uint8_t)color;
        }
        break;
    case PIXFMT_RGB565:
        for (int yy = y; yy < y + h; yy++) {
            offset = (yy * dst->width + x) * 2;
            ptr16 = (uint16_t *)&(dst->buf[offset]);
            for (int xx = 0; xx < w; xx++)
                *ptr16++ = (uint16_t)color;
        }
        break;
    case PIXFMT_RGB888:
    case PIXFMT_ARGB8888:
    case PIXFMT_RGBA8888:
        fprintf(stderr, "Unimplemented function: Fill 8bpc buffer\n");
        break;
    }
}

void os_disp_bilt(
        Canvas *dst,
        Canvas *src,
        int dst_x, int dst_y, int dst_w, int dst_h,
        int src_x, int src_y, int src_w, int src_h) {
    fprintf(stderr, "Unimplemented function: Bilt buffer\n");
}

void os_disp_bilt_raw(
        Canvas *dst, uint8_t *src,
        int dst_x, int dst_y,
        int src_x, int src_y, int src_w, int src_h) {
    // This function requires the src to have the same pixel format as the
    // destination canvas (implied from parameter list)
    int bpp = os_disp_get_bpp(dst->pixelFormat);
    if (bpp < 8) {
        fprintf(stderr, "Unimplemented function: blit monochrome buffer\n");
    }
    int pixel_bytes = bpp /8;
    int src_line_bytes = src_w * pixel_bytes;
    int dst_line_bytes = dst->width * pixel_bytes;
    for (int y = 0; y < src_h; y++) {
        uint8_t *p_dst = dst->buf + (dst_y + y) * dst_line_bytes +
                dst_x * pixel_bytes;
        uint8_t *p_src = src + y * src_line_bytes + src_x * pixel_bytes;
        memcpy(p_dst, p_src, src_line_bytes);
    } 
}

uint32_t os_disp_conv_pix(PixelFormat dst, PixelFormat src, uint32_t color) {
    int r = 0x00, g = 0x00, b = 0x00, a = 0xff;

    if (src == dst)
        return color;

    switch (src) {
    case PIXFMT_Y1:
    case PIXFMT_Y2:
    case PIXFMT_Y4:
    case PIXFMT_Y8:
        r = g = b = os_disp_pix_to_y(color, src);
        break;
    case PIXFMT_RGB565:
        r = (color >> 8) & 0xf8;
        g = (color >> 3) & 0xfc;
        b = (color << 3) & 0xf8;
        r |= r >> 5;
        g |= g >> 6;
        b |= b >> 5;
        break;
    case PIXFMT_ARGB8888:
        a = (color >> 24) & 0xff; __attribute__ ((fallthrough));
    case PIXFMT_RGB888:
        r = (color >> 16) & 0xff;
        g = (color >> 8) & 0xff;
        b = (color) & 0xff;
        break;
    case PIXFMT_RGBA8888:
        r = (color >> 24) & 0xff;
        g = (color >> 16) & 0xff;
        b = (color >> 8) & 0xff;
        a = (color) & 0xff;
        break;
    }

    uint8_t y = (uint8_t)((r + g + b) / 3);
    uint32_t target = 0;
    switch (dst) {
    case PIXFMT_Y1:
        target = (y >> 7) & 0x1;
        break;
    case PIXFMT_Y2:
        target = (y >> 6) & 0x3;
        break;
    case PIXFMT_Y4:
        target = (y >> 4) & 0xf;
        break;
    case PIXFMT_Y8:
        target = y;
        break;
    case PIXFMT_RGB565:
        target = ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | ((b & 0xf8) >> 3);
        break;
    case PIXFMT_ARGB8888:
        target = (a << 24) | (r << 16) | (g << 8) | b;
        break;
    case PIXFMT_RGB888:
        target = (r << 16) | (g << 8) | b;
        break;
    case PIXFMT_RGBA8888:
        target = (r << 24) | (g << 16) | (b << 8) | a;
        break;
    }
    return target;
}

void os_disp_conv(Canvas *dst, Canvas *src) {
    fprintf(stderr, "Unimplemented function: buffer format conversion\n");
}

// Commit to screen
void os_disp_draw(Canvas *src) {
    if (src->pixelFormat == PIXFMT_RGB565) {
        memcpy(screen->pixels, src->buf, DISP_WIDTH * DISP_HEIGHT * 2);
    }
    else {
        for (int yy = 0; yy < DISP_HEIGHT; yy++) {
            for (int xx = 0; xx < DISP_WIDTH; xx++)
                ((uint32_t *)screen->pixels)[yy * DISP_WIDTH + xx] = 
                        os_disp_conv_pix(PIXFMT_ARGB8888, src->pixelFormat,
                        os_disp_get(src, xx, yy));
        }
    }
    SDL_Flip(screen);
}
