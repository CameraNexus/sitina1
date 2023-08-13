//
// Sitina1
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
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "image_processing.h"
#include "ccd_timing.h"

static uint16_t max = 0;
static uint16_t min = 65535;

#define BLACK_LEVEL 1800
#define FULL_LEVEL 55000

static uint16_t ip_scale_pixel(uint16_t pixel) {
    // if (pixel < min)
    //     min = pixel;
    // if (pixel > max)
    //     max = pixel;
    if (pixel < BLACK_LEVEL)
        pixel = BLACK_LEVEL;
    else
        pixel -= BLACK_LEVEL;
    pixel /= (FULL_LEVEL - BLACK_LEVEL) / 255;
    return pixel;
}

static uint16_t y2rgb565(uint16_t y) {
    return ((y >> 3) << 11) | ((y >> 2) << 5) | (y >> 3);
}

static uint32_t y2rgb888(uint32_t y) {
    return (0xff000000ul | (y << 16) | (y << 8) | (y));
}

static uint32_t populate_pixel(uint16_t a, uint16_t b, uint8_t x_odd, uint8_t y_odd) {
    if (y_odd) {
        if (x_odd) {
            // RG
            return (0xff000000ul | (b << 16) | (a << 8));
        }
        else {
            // GR
            return (0xff000000ul | (a << 16) | (b << 8));
        }
    }
    else {
        if (x_odd) {
            // GB
            return (0xff000000ul | (b << 8) | (a));
        }
        else {
            // BG
            return (0xff000000ul | (a << 8) | (b));
        }
    }
} 

// This function takes draft input and rescale to output as 720x480 image
void ip_filter_draft_image(uint16_t *inp, uint32_t *outp, int offset) {
    int y_offset = 3;
    int x_offset = (CCD_PRV_HBLK_LENGTH + CCD_DUMMY_PIX + CCD_DARK_PIX + CCD_BUFFER_PIX + 40) * 2;
    // 248 lines, drop 8 lines
    for (int y = 0; y < 240; y++) {
        //printf("Black level: %d\n", inp[(y + y_offset) * CCD_PRV_LINE_LENGTH * 2 + x_offset - 80]);
        // For active 4008 pixels, output is 720 pixels.
        // Similar to V, where 10 lines are skipped every 11 lines
        // This means 10 pixels to be skipped every 11 pixels
        // In our case we use 2 pixels every 11 pixels, though the color would be different
        uint16_t *rdptr = &inp[(y + y_offset) * CCD_PRV_LINE_LENGTH * 2 + x_offset];
        uint8_t y_odd = y & 0x1;
        for (int x = 0; x < 180; x++) {
            // Sample the following
            // x+0: B
            // x+1: G
            // x+2: skip
            // x+3: skip
            // x+4: skip
            // x+5: skip
            // x+6: B
            // x+7: G
            // x+8: skip
            // x+9: skip
            // x+10:skip
            // This is slightly skewed
            uint16_t p0a = ip_scale_pixel(*rdptr++);
            uint16_t p3b = ip_scale_pixel(*rdptr++);
            uint16_t p0b = ip_scale_pixel(*rdptr++);
            uint16_t p3a = ip_scale_pixel(*rdptr++);
            rdptr += 8;
            uint16_t p1a = ip_scale_pixel(*rdptr++);
            uint16_t p2b = ip_scale_pixel(*rdptr++);
            uint16_t p1b = ip_scale_pixel(*rdptr++);
            uint16_t p2a = ip_scale_pixel(*rdptr++);
            rdptr += 6;
            uint8_t x_odd = x & 0x1;
            outp[y * 2 * 720 + (360 - x) * 2 - 1] = populate_pixel(p0a, p0b, x_odd, y_odd);
            outp[y * 2 * 720 + (360 - x) * 2 - 2] = populate_pixel(p1a, p1b, x_odd, y_odd);;
            outp[y * 2 * 720 + x * 2 + 0] = populate_pixel(p3a, p3b, x_odd, y_odd);
            outp[y * 2 * 720 + x * 2 + 1] = populate_pixel(p2a, p2b, x_odd, y_odd);
        }
        // Filling missing color
        // Run every 2 lines
        if (y_odd) {
            for (int i = 0; i < 2; i++) {
                int yy = (y & ~0x1) | i;
                for (int x = 0; x < 720; x++) {
                    uint32_t inpix = outp[yy * 2 * 720 + x];
                    uint32_t nbpix = outp[(yy ^ 0x1) * 2 * 720 + x];
                    uint8_t x_odd = x & 0x2;
                    uint32_t outpix = inpix;
                    if (yy & 0x1) {
                        // RG or GR
                        outpix |= nbpix & 0xfful;
                    }
                    else {
                        // GB or BG
                        outpix |= nbpix & 0xff0000ul;
                    }
                    outp[yy * 2 * 720 + x] = outpix;
                }
                // Line doubling
                memcpy(&outp[(yy * 2 + 1) * 720], &outp[(yy * 2) * 720], 4 * 720);
            }
        }
    }

    // printf("min: %d\n", min);
    // printf("max: %d\n", max);
}
