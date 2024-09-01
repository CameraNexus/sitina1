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
#include "softi2c.h"
#include "mcusvc.h"
#include "ccd_timing.h"
#include "afe.h" // debug
#include "dcif.h" // For now, the buffer size should match image size
#include "pal_camera.h"
#include "pal_display.h"
#include "pal_power.h"
#include "pal_input.h"
#include "image_processing.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "xparameters.h"
#include "xscugic.h"
#include "font.h"

extern const unsigned char gImage_image480480[921600];
uint32_t previewbuf[PROC_PREVIEW_HEIGHT * PROC_PREVIEW_WIDTH];

static void lcd_set_pixel(uint32_t *buf, int x, int y, int c) {
    x = 479 - x;
    y = 479 - y;
    buf[y * 480 + x] = (c) ? 0xffffffff : 0x00000000;
}

void lcd_disp_char(uint32_t *buf, int x, int y, char c) {
    for (int yy = 0; yy < 12; yy++) {
        for (int xx = 0; xx < 8; xx++) {
            if ((charMap_ascii[(uint32_t)c][yy] >> xx) & 0x01) {
                lcd_set_pixel(buf, x + xx, y + yy, 1);
            }
            else {
                lcd_set_pixel(buf, x + xx, y + yy, 0);
            }
        }
    }
}

void lcd_disp_string(uint32_t *buf, int x, int y, char *str) {
    while (*str) {
        lcd_disp_char(buf, x, y, *str);
        x += 8;
        str++;
    }
}

extern uint16_t maxlvl;
extern uint16_t minlvl;

int main()
{
    i2c_init();
    si2c_init();
    mcusvc_init();

    int retval = si2c_write_reg(SI2C0, 0x41, 0x00, 0x8f);
    if (retval < 0) {
        printf("SI2C write failed!\n");
    }
    retval = si2c_write_reg(SI2C1, 0x41, 0x00, 0x8f);
    if (retval < 0) {
        printf("SI2C write failed!\n");
    }

    pal_pwr_init();
    pal_cam_init();
    pal_disp_init();
    pal_input_init();
    
    pal_pwr_set_vab(127);

    pal_cam_start();

    /*while (1) {
        mcusvc_set_led(true);
        usleep(500*1000);
        mcusvc_set_led(false);
        usleep(500*1000);
    }*/

    uint32_t cnt0 = 15;
    uint32_t cnt1 = 34;
    uint32_t cnt2 = 67;
    bool update = true;

    ip_init();

    while (1) {
        uint32_t *scrbuf = pal_disp_get_buffer();
        pal_input_scan();

        uint16_t *cambuf = pal_cam_get_full_buffer();
        mcusvc_set_led(true);

        ip_filter_draft_image(cambuf, previewbuf);
        
        //memset(scrbuf, 0x00, 480*480*4);
        uint32_t *wrptr = scrbuf;
        for (int y = 0; y < 336; y++) {
            for (int x = 0; x < 480; x++) {
                uint32_t pix = previewbuf[y * PROC_DRAFT_WIDTH + x];
                *wrptr++ = pix;
            }
        }
        /*for (int y = 0; y < 337; y++) {
            for (int x = 0; x < 480; x++) {
                uint32_t pix;
                if (x >= 240) {
                    // when X = 240, X = 239*16+252+1 = 4077
                    // when X = 239, X = 4076
                    pix = (uint32_t)cambuf[y * 8 * CAM_HACT + (239 - (x - 240)) * 16 + 252 + 1];
                    //pix = (uint32_t)cambuf[y * 8 * CAM_HACT + (239 * 8 - (x - 240)) * 2 + 250];
                }
                else {
                    pix = (uint32_t)cambuf[y * 8 * CAM_HACT + x * 16 + 252];
                }
                
                pix >>= 8;
                pix &= 0xff;
                pix |= (pix << 8) | (pix << 16);
                *wrptr++ = pix;
            }
        }*/

        uint32_t btns = pal_input_get_keys();
        if (btns & 0x100) {
            // up
            if (cnt0 < 63)
                cnt0 ++;
            update = true;
        }
        else if (btns & 0x800) {
            // down
            if (cnt0 > 0)
                cnt0 --;
            update = true;
        }
        if (btns & 0x200) {
            // left
            if (cnt1 > 0)
                cnt1 --;
            update = true;
        }
        else if (btns & 0x400) {
            // right
            if (cnt1 < 63)
                cnt1 ++;
            update = true;
        }
        if (btns & 0x1000) {
            if (cnt2 > 3)
                cnt2 -= 4;
            update = true;
        }
        else if (btns & 0x2000) {
            if (cnt2 < 1020)
                cnt2 += 4;
            update = true;
        }
        char strbuf[24];
        snprintf(strbuf, 15, "SHDL %d", cnt0);
        lcd_disp_string(scrbuf, 0, 0, strbuf);
        snprintf(strbuf, 15, "SHPL %d", cnt1);
        lcd_disp_string(scrbuf, 0, 16, strbuf);
        uint32_t dbx10000 = 57600 + 358 * cnt2;
        snprintf(strbuf, 23, "GAIN %d (%d.%02d dB)", cnt2, dbx10000/ 10000, (dbx10000 % 10000) / 100);
        lcd_disp_string(scrbuf, 0, 32, strbuf);

        snprintf(strbuf, 15, "MIN %d", minlvl);
        lcd_disp_string(scrbuf, 0, 48, strbuf);
        snprintf(strbuf, 15, "MAX %d", maxlvl);
        lcd_disp_string(scrbuf, 0, 64, strbuf);
        minlvl = 65535;
        maxlvl = 0;
        // snprintf(strbuf, 15, "CLPOB %d", cnt2);
        // lcd_disp_string(scrbuf, 0, 32, strbuf);

        if (update) {
            afe_write_reg(0x38,
                (cnt0 << 0) | // SHDLOC
                (cnt1 << 8) | // SHPLOC
                (cnt2 << 16)); // SHPWIDTH
            afe_debug(cnt2);
            update = false;
        }
        
        //memset(cambuf, 0xff, CAM_BUFSIZE);
        //Xil_DCacheFlushRange(cambuf, CAM_BUFSIZE);
        pal_disp_return_buffer(scrbuf);
        mcusvc_set_led(false);
    }

    return 0;
}
