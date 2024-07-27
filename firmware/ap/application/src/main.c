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
#include <stdio.h>
#include <stdbool.h>
#include "i2c.h"
#include "mcusvc.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xscugic.h"

extern uint8_t framebuffer[];

void lcd_set_color(uint32_t color) {
    uint32_t *wrptr = framebuffer;
    for (int i = 0; i < 480*480; i++) {
        *wrptr++ = color;
    }
    Xil_DCacheFlushRange((intptr_t)framebuffer, 480*480*4);
}

int main()
{
    i2c_init();
    mcusvc_init();
    lcd_init();
    //memset(s_image_buffer, 0xFF, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);

    //memcpy(s_image_buffer, gImage_image480480, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);
    //Xil_DCacheFlushRange((UINTPTR)s_image_buffer, IMAGE_WIDTH * IMAGE_HEIGHT * IMAGE_PIXEL_SIZE);

    for(;;) {
    	sleep(1);
        mcusvc_set_led(true);
        lcd_set_color(0xffff0000);
        sleep(1);
        mcusvc_set_led(false);
        lcd_set_color(0xff00ff00);
        sleep(1);
        lcd_set_color(0xff0000ff);
    // memset(framebuffer, 0xff, 480*4);
    // memset(framebuffer[480*4*2], 0xff, 480*4);
    // for (int i = 0; i < 240; i++) {
    //     framebuffer[480*4*4+i*2*4 + 0] = 0xff;
    //     framebuffer[480*4*4+i*2*4 + 1] = 0xff;
    //     framebuffer[480*4*4+i*2*4 + 2] = 0xff;
    //     framebuffer[480*4*4+i*2*4 + 3] = 0xff;
    // }

    
    }

    return 0;
}
