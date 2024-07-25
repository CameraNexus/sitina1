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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "xil_cache.h"
#include "lcd.h"
#include "gpio_lp.h"
#include "mu_platform.h"

#define IMAGE_WIDTH  	 480
#define IMAGE_HEIGHT 	 480
#define IMAGE_PIXEL_SIZE 4

#define DMACTL_BURSTLEN         8   // in 64-bit words
#define DMACTL_MAXINFLIGHT      4   // Max number of outstanding requests

#define DCCTL_SWAP_32B          0x4
#define DCCTL_TGEN_EN           0x1

extern const unsigned char gImage_image480480[921600];

#define FRAMEBUF_SIZE   IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE
uint8_t __aligned(64) framebuffer[FRAMEBUF_SIZE];

static uint8_t s_lcd_init_seq[] = {
		0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x13,
		0xEF,  1, 0x08,
		0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x10,
		0xC0,  2, 0x3B, 0x00,
		0xC1,  2, 0x10, 0x0C,
		0xC2,  2, 0x37, 0x0A, // Inversion selection 31-2dot, 37-column
		0xC3,  1, 0x02,
		0xCC,  1, 0x10,
		0xCD,  1, 0x08,
	    0xB0, 16, 0x40, 0x0E, 0x58, 0x0E, 0x12, 0x08, 0x0C, 0x09, 0x09, 0x27, 0x07, 0x18, 0x15, 0x78, 0x26, 0xC7,
	    0xB1, 16, 0x40, 0x13, 0x5B, 0x0D, 0x11, 0x06, 0x0A, 0x08, 0x08, 0x26, 0x03, 0x13, 0x12, 0x79, 0x28, 0xC9,
	    0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x11,
	    0xB0,  1, 0x6D, //30
	    0xB1,  1, 0x38, //VCOM
	    0xB2,  1, 0x81, //84
	    0xB3,  1, 0x80,
	    0xB5,  1, 0x4E, //4E
	    0xB7,  1, 0x85,
	    0xB8,  1, 0x20,
	    0xC1,  1, 0x78,
	    0xC2,  1, 0x78,
	    0xD0,  1, 0x88,
	    0xE0,  3, 0x00, 0x00, 0x02,
	    0xE1, 11, 0x06, 0x30, 0x08, 0x30, 0x05, 0x30, 0x07, 0x30, 0x00, 0x33, 0x33,
	    0xE2, 12, 0x11, 0x11, 0x33, 0x33, 0xF4, 0x00, 0x00, 0x00, 0xF4, 0x00, 0x00, 0x00,
	    0xE3,  4, 0x00, 0x00, 0x11, 0x11,
	    0xE4,  2, 0x44, 0x44,
	    0xE5, 16, 0x0D, 0xF5, 0x30, 0xF0, 0x0F, 0xF7, 0x30, 0xF0, 0x09, 0xF1, 0x30, 0xF0, 0x0B, 0xF3, 0x30, 0xF0,
	    0xE6,  4, 0x00, 0x00, 0x11, 0x11,
	    0xE7,  2, 0x44, 0x44,
	    0xE8, 16, 0x0C, 0xF4, 0x30, 0xF0, 0x0E, 0xF6, 0x30, 0xF0, 0x08, 0xF0, 0x30, 0xF0, 0x0A, 0xF2, 0x30, 0xF0,
	    0xE9,  2, 0x36, 0x01,
	    0xEB,  7, 0x00, 0x01, 0xE4, 0xE4, 0x44, 0x88, 0x40,
	    0xED, 16, 0xFF, 0x45, 0x67, 0xFA, 0x01, 0x2B, 0xCF, 0xFF, 0xFF, 0xFC, 0xB2, 0x10, 0xAF, 0x76, 0x54, 0xFF,
	    0xEF,  6, 0x10, 0x0D, 0x04, 0x08, 0x3F, 0x1F,
	    0xFF,  5, 0x77, 0x01, 0x00, 0x00, 0x00,
	    0x11,  0
};

static void dsi_initseq(void) {
	gpio_lp_write_dcs(0x01, NULL, 0x00);

	usleep(200 * 1000);

	uint32_t i = 0;
	while(i < sizeof(s_lcd_init_seq)) {
		gpio_lp_write_dcs(s_lcd_init_seq[i], &s_lcd_init_seq[i + 2], s_lcd_init_seq[i + 1]);
		i += s_lcd_init_seq[i + 1] + 2; // Next seq
	}

	usleep(200 * 1000);

	gpio_lp_write_dcs(0x29, NULL, 0x00);

	usleep(25 * 1000);

	gpio_lp_write_dcs(0x35, NULL, 0x00);

    //gpio_lp_write_dcs(0x22, NULL, 0x00);
}

void lcd_init(void) {
	gpio_lp_init();
	gpio_lp_setup();

	// Return all lanes to LP-11 state
	usleep(200 * 1000);

	dsi_initseq();

	gpio_lp_release();

    memcpy(framebuffer, gImage_image480480, FRAMEBUF_SIZE);

    memset(framebuffer, 0, 480*4*10);
    memset(framebuffer, 0xff, 480*4);
    memset(framebuffer[480*4*2], 0xff, 480*4);
    for (int i = 0; i < 240; i++) {
        framebuffer[480*4*4+i*2*4 + 0] = 0xff;
        framebuffer[480*4*4+i*2*4 + 1] = 0xff;
        framebuffer[480*4*4+i*2*4 + 2] = 0xff;
        framebuffer[480*4*4+i*2*4 + 3] = 0xff;
    }

    Xil_DCacheFlushRange((intptr_t)framebuffer, FRAMEBUF_SIZE);

    *DSILITE_STARTADDR = (uint32_t)framebuffer;
    *DSILITE_ENDADDR = (uint32_t)framebuffer + FRAMEBUF_SIZE;
    *DSILITE_DMACTL = (DMACTL_MAXINFLIGHT << 8) | DMACTL_BURSTLEN;
    *DSILITE_PCTL = DCCTL_TGEN_EN;
}
