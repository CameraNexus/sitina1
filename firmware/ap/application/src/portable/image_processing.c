//
// Sitina 1
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
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "ccd_timing.h"
#include "image_processing.h"

#define BLACK_LEVEL 1800
#define FULL_LEVEL  65535 // Ideally close to 65535

#define GAMMA   2.2

// for debugging purposes
uint16_t maxlvl = 0;
uint16_t minlvl = 65535;

//static uint16_t degamma_table[256];
static uint8_t gamma_table[1024];
static uint32_t histo_tmp[256];

static void build_gamma_table(void) {
    for (int i = 0; i < 1024; i++) {
        //degamma_table[i] = (uint16_t)(powf(i / 255.0f, GAMMA) * 1023.0f);
        int32_t srgbi = powf(i / 1023.0f, 1.0f / GAMMA) * 255.0f;
        if (srgbi > 255) srgbi = 255;
        if (srgbi < 0) srgbi = 0;
        gamma_table[i] = srgbi;
    }
}

static uint16_t ip_scale_pixel(uint16_t pixel) {
    if (pixel < minlvl)
        minlvl = pixel;
    if (pixel > maxlvl)
        maxlvl = pixel;
    if (pixel < BLACK_LEVEL)
        pixel = 0;
    else
        pixel -= BLACK_LEVEL;
    pixel /= (FULL_LEVEL - BLACK_LEVEL) / 1023;
    if (pixel > 1023)
        pixel = 1023;
    return gamma_table[pixel];
}

static uint16_t y2rgb565(uint16_t y) {
    return ((y >> 3) << 11) | ((y >> 2) << 5) | (y >> 3);
}

static uint32_t y2rgb888(uint32_t y) {
    return (0xff000000ul | (y << 16) | (y << 8) | (y));
}

void ip_filter_draft_image(uint16_t *in, uint32_t *out, uint8_t *histogram) {
    // Skip 4 lines
    /*uint16_t *evenptr = in + PROC_IN_SKIP_LINE * PROC_IN_WIDTH + PROC_IN_SKIP;
    uint16_t *oddptr = evenptr + PROC_IN_WIDTH;
    uint32_t *leftptr = out;
    uint32_t *rightptr = out + PROC_DRAFT_WIDTH - 1;
    memset(histo_tmp, 0, 256 * 4);
    for (int i = 0; i < PROC_DRAFT_HEIGHT; i++) {
        for (int j = 0; j < PROC_DRAFT_WIDTH / 2; j++) {
            uint8_t p0b = ip_scale_pixel(*evenptr++);
            uint8_t p1g0 = ip_scale_pixel(*evenptr++);
            uint8_t p0g0 = ip_scale_pixel(*evenptr++);
            uint8_t p1b = ip_scale_pixel(*evenptr++);
            uint8_t p0g1 = ip_scale_pixel(*oddptr++);
            uint8_t p1r = ip_scale_pixel(*oddptr++);
            uint8_t p0r = ip_scale_pixel(*oddptr++);
            uint8_t p1g1 = ip_scale_pixel(*oddptr++);
            //uint8_t p0g = ((uint16_t)p0g0 + (uint16_t)p0g1) / 2;
            //uint8_t p1g = ((uint16_t)p1g0 + (uint16_t)p1g1) / 2;
            uint8_t p0g = p0g0;
            uint8_t p1g = p1g0;
            uint32_t p0 = ((uint32_t)p0r << 16) | ((uint32_t)p0g << 8) | p0b;
            uint32_t p1 = ((uint32_t)p1r << 16) | ((uint32_t)p1g << 8) | p1b;

            // Incorrect but fast RGB to Y
            uint8_t p0y = (p0g0 >> 1) + (p0b >> 2) + (p0r >> 2);
            uint8_t p1y = (p1g0 >> 1) + (p1b >> 2) + (p1r >> 2);
            histo_tmp[p0y]++;
            histo_tmp[p1y]++;

            *leftptr++ = p0;
            *rightptr-- = p1;
            //*leftptr++ = 0x00ff0000;
            //*rightptr-- = 0x0000ff00;
            evenptr += (4 * 3); // skip 3 pixels
            oddptr += (4 * 3);
        }
        oddptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END + 4; // Skip line
        evenptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END + 4;
        if (i % 3 == 0) {
            oddptr += (PROC_IN_WIDTH) * 2; // skip additional lines
            evenptr += (PROC_IN_WIDTH) * 2;
        }
        
        leftptr += (PROC_DRAFT_WIDTH / 2);
        rightptr += (PROC_DRAFT_WIDTH / 2) + PROC_DRAFT_WIDTH;
    }

    uint32_t hmax = 0;
    for (int i = 0; i < 256; i++) {
        if (hmax < histo_tmp[i])
            hmax = histo_tmp[i];
    }
    for (int i = 0; i < 256; i++) {
        histogram[i] = histo_tmp[i] * 255 / hmax;
    }*/
    uint16_t *evenptr = in + PROC_IN_SKIP_LINE * PROC_IN_WIDTH + PROC_IN_SKIP;
    uint16_t *oddptr = evenptr + PROC_IN_WIDTH;
    uint32_t *leftptr = out;
    uint32_t *rightptr = out + PROC_DRAFT_WIDTH - 1;
    memset(histo_tmp, 0, 256 * 4);
    for (int i = 0; i < PROC_DRAFT_HEIGHT; i++) {
        for (int j = 0; j < PROC_DRAFT_WIDTH / 2; j++) {
            uint8_t p0b = ip_scale_pixel(*evenptr++);
            uint8_t p1g0 = ip_scale_pixel(*evenptr++);
            uint8_t p0g0 = ip_scale_pixel(*evenptr++);
            uint8_t p1b = ip_scale_pixel(*evenptr++);
            uint8_t p0g1 = ip_scale_pixel(*oddptr++);
            uint8_t p1r = ip_scale_pixel(*oddptr++);
            uint8_t p0r = ip_scale_pixel(*oddptr++);
            uint8_t p1g1 = ip_scale_pixel(*oddptr++);
            //uint8_t p0g = ((uint16_t)p0g0 + (uint16_t)p0g1) / 2;
            //uint8_t p1g = ((uint16_t)p1g0 + (uint16_t)p1g1) / 2;
            uint8_t p0g = p0g0;
            uint8_t p1g = p1g0;
            uint32_t p0 = ((uint32_t)p0r << 16) | ((uint32_t)p0g << 8) | p0b;
            uint32_t p1 = ((uint32_t)p1r << 16) | ((uint32_t)p1g << 8) | p1b;

            // Incorrect but fast RGB to Y
            uint8_t p0y = (p0g0 >> 1) + (p0b >> 2) + (p0r >> 2);
            uint8_t p1y = (p1g0 >> 1) + (p1b >> 2) + (p1r >> 2);
            histo_tmp[p0y]++;
            histo_tmp[p1y]++;

            *leftptr++ = p0;
            *rightptr-- = p1;
            //*leftptr++ = 0x00ff0000;
            //*rightptr-- = 0x0000ff00;
            evenptr += (4 * 3); // skip 3 pixels
            oddptr += (4 * 3);
        }
        oddptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END; // Skip line
        evenptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END;
        oddptr += (PROC_IN_WIDTH) * 6 + 4; // skip additional lines
        evenptr += (PROC_IN_WIDTH) * 6 + 4;
        leftptr += (PROC_DRAFT_WIDTH / 2);
        rightptr += (PROC_DRAFT_WIDTH / 2) + PROC_DRAFT_WIDTH;
    }

    uint32_t hmax = 0;
    for (int i = 0; i < 256; i++) {
        if (hmax < histo_tmp[i])
            hmax = histo_tmp[i];
    }
    for (int i = 0; i < 256; i++) {
        histogram[i] = histo_tmp[i] * 255 / hmax;
    }
}

void ip_filter_preview_image(uint16_t *in, uint32_t *out, uint8_t *histogram) {
    /*// interleaved input
    uint16_t *evenptr = in + PROC_IN_SKIP_LINE * PROC_IN_WIDTH + PROC_IN_SKIP;
    uint16_t *oddptr = evenptr + PROC_IN_WIDTH;
    uint32_t *leftptr = out;
    uint32_t *rightptr = out + PROC_PREVIEW_WIDTH - 1;
    for (int i = 0; i < PROC_PREVIEW_HEIGHT; i++) {
        for (int j = 0; j < PROC_PREVIEW_WIDTH / 2; j++) {
            uint8_t p0b = ip_scale_pixel(*evenptr++);
            uint8_t p1g0 = ip_scale_pixel(*evenptr++);
            uint8_t p0g0 = ip_scale_pixel(*evenptr++);
            uint8_t p1b = ip_scale_pixel(*evenptr++);
            uint8_t p0g1 = ip_scale_pixel(*oddptr++);
            uint8_t p1r = ip_scale_pixel(*oddptr++);
            uint8_t p0r = ip_scale_pixel(*oddptr++);
            uint8_t p1g1 = ip_scale_pixel(*oddptr++);
            //uint8_t p0g = ((uint16_t)p0g0 + (uint16_t)p0g1) / 2;
            //uint8_t p1g = ((uint16_t)p1g0 + (uint16_t)p1g1) / 2;
            uint8_t p0g = p0g0;
            uint8_t p1g = p1g0;
            uint32_t p0 = ((uint32_t)p0r << 16) | ((uint32_t)p0g << 8) | p0b;
            uint32_t p1 = ((uint32_t)p1r << 16) | ((uint32_t)p1g << 8) | p1b;
            *leftptr++ = p0;
            *rightptr-- = p1;
            //*leftptr++ = 0x00ff0000;
            //*rightptr-- = 0x0000ff00;
        }
        oddptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END; // Skip line
        evenptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END;
        leftptr += (PROC_PREVIEW_WIDTH / 2);
        rightptr += (PROC_PREVIEW_WIDTH / 2) + PROC_PREVIEW_WIDTH;
    }*/
    uint16_t *evenptr = in + PROC_IN_SKIP_LINE * PROC_IN_WIDTH + PROC_IN_SKIP;
    uint16_t *oddptr = evenptr + PROC_IN_WIDTH;
    uint32_t *leftptr = out;
    uint32_t *rightptr = out + PROC_DRAFT_WIDTH - 1;
    memset(histo_tmp, 0, 256 * 4);
    for (int i = 0; i < PROC_DRAFT_HEIGHT; i++) {
        for (int j = 0; j < PROC_DRAFT_WIDTH / 2; j++) {
            uint8_t p0b = ip_scale_pixel(*evenptr++);
            uint8_t p1g0 = ip_scale_pixel(*evenptr++);
            uint8_t p0g0 = ip_scale_pixel(*evenptr++);
            uint8_t p1b = ip_scale_pixel(*evenptr++);
            uint8_t p0g1 = ip_scale_pixel(*oddptr++);
            uint8_t p1r = ip_scale_pixel(*oddptr++);
            uint8_t p0r = ip_scale_pixel(*oddptr++);
            uint8_t p1g1 = ip_scale_pixel(*oddptr++);
            //uint8_t p0g = ((uint16_t)p0g0 + (uint16_t)p0g1) / 2;
            //uint8_t p1g = ((uint16_t)p1g0 + (uint16_t)p1g1) / 2;
            uint8_t p0g = p0g0;
            uint8_t p1g = p1g0;
            uint32_t p0 = ((uint32_t)p0r << 16) | ((uint32_t)p0g << 8) | p0b;
            uint32_t p1 = ((uint32_t)p1r << 16) | ((uint32_t)p1g << 8) | p1b;

            // Incorrect but fast RGB to Y
            uint8_t p0y = (p0g0 >> 1) + (p0b >> 2) + (p0r >> 2);
            uint8_t p1y = (p1g0 >> 1) + (p1b >> 2) + (p1r >> 2);
            histo_tmp[p0y]++;
            histo_tmp[p1y]++;

            *leftptr++ = p0;
            *rightptr-- = p1;
            //*leftptr++ = 0x00ff0000;
            //*rightptr-- = 0x0000ff00;
            evenptr += (4 * 3); // skip 3 pixels
            oddptr += (4 * 3);
        }
        oddptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END; // Skip line
        evenptr += PROC_IN_WIDTH + PROC_IN_SKIP + PROC_IN_SKIP_END;
        oddptr += (PROC_IN_WIDTH) * 6 + 4; // skip additional lines
        evenptr += (PROC_IN_WIDTH) * 6 + 4;
        leftptr += (PROC_DRAFT_WIDTH / 2);
        rightptr += (PROC_DRAFT_WIDTH / 2) + PROC_DRAFT_WIDTH;
    }

    uint32_t hmax = 0;
    for (int i = 0; i < 256; i++) {
        if (hmax < histo_tmp[i])
            hmax = histo_tmp[i];
    }
    for (int i = 0; i < 256; i++) {
        histogram[i] = histo_tmp[i] * 255 / hmax;
    }
}

void ip_init(void) {
    build_gamma_table();
}
