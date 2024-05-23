//
// Sitina simulator
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
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "camsim.h"
#include "stb_image.h"
#include "stb_image_resize.h"

#define CAM_WIDTH   128
#define CAM_HEIGHT  128

static uint16_t pixels[CAM_WIDTH * CAM_HEIGHT];
static int x_counter;
static int y_counter;
static int frame_counter;

void print_buffer(uint8_t *buf, size_t bytes) {
    for (size_t i = 0; i < bytes / 16; i++) {
        for (size_t j = 0; j < 16; j++) {
            printf("%02x ", buf[i*16+j]);
        }
        printf("\n");
    }
}

static void camsim_next_frame() {
#if 1 // fixed border
    memset(pixels, 0xff, CAM_WIDTH * CAM_HEIGHT * 2);
    for (int i = 0; i < CAM_WIDTH; i++) {
        pixels[i] = 0x00;
        pixels[(CAM_HEIGHT - 1) * CAM_WIDTH + i] = 0x00;
    }
    for (int i = 0; i < CAM_HEIGHT; i++) {
        pixels[CAM_WIDTH * i] = 0x00;
        pixels[CAM_WIDTH * i + CAM_WIDTH - 1] = 0x00;
    }
#endif
}

void camsim_reset() {
    x_counter = 0;
    y_counter = 0;
    camsim_next_frame();
}

void camsim_apply(uint8_t &hsync, uint8_t &vsync, uint16_t &d, uint8_t &clk) {
    vsync = (y_counter == 0) ? 1 : 0;
    hsync = (x_counter == 0) ? 1 : 0;

    d = pixels[y_counter * CAM_WIDTH + x_counter++];

    if (x_counter == CAM_WIDTH) {
        x_counter = 0;
        y_counter++;
        if (y_counter == CAM_HEIGHT) {
            y_counter = 0;
            camsim_next_frame();
        }
    }
}