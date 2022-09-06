//
// Project Sitina
// Copyright 2022 Wenting Zhang
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
#include <string.h>
#include <stdint.h>
#include "os_display.h"
#include "os_filesystem.h"
#include "os_input.h"
#include "os_camera.h"
#include "ccd_timing.h"
#include "font.h"

static Canvas *screen;

void app_init() {
    os_input_init();
    os_disp_init();
    font_init();
    //os_disp_enter_sleep();
    screen = os_disp_create(DISP_WIDTH, DISP_HEIGHT, PIXFMT_RGB565);
    os_disp_fill(screen, 0, 0, DISP_WIDTH, DISP_HEIGHT, 0x0000);
    font_set_font(FNT_16);
    font_disp(screen, 0, 0, DISP_WIDTH, 0xFFFF, "System starting up...", 64, CE_UTF8);
    os_disp_draw(screen);
    os_fs_init();
    font_disp(screen, 0, 16, DISP_WIDTH, 0xFFFF, "Storage subsystem initialized.", 64, CE_UTF8);
    os_disp_draw(screen);
    os_camera_init();
    font_disp(screen, 0, 32, DISP_WIDTH, 0xFFFF, "Imaging subsystem initialized.", 64, CE_UTF8);
    os_disp_draw(screen);
}

static uint32_t app_wait_key() {
    uint32_t key_state;
    uint32_t temp;

    printf("Waiting for key\n");

    do {
        os_input_poll();
        key_state = os_input_get_keys();
    } while (key_state == 0);

    printf("Got key %08x\n", key_state);

    do {
        os_input_poll();
        temp = os_input_get_keys();
    } while (temp != 0);

    printf("Done\n");

    return key_state;
}

static void app_disp_captured_image() {
    // Completely wrong actually
    uint16_t *camera_buffer = os_camera_get_buffer();
    for (int y = 0; y < CCD_FIELD_LINES / 7; y++) {
        for (int x = 0; x < CCD_LINE_LENGTH * 2 / 7; x++) {
            uint16_t src_pixel = camera_buffer[y * 7 * (CCD_LINE_LENGTH * 2) + x * 7];
            uint16_t dst_pixel = src_pixel;
            os_disp_set(screen, x, y, dst_pixel);
        }
    }
}

void app_main() {
    uint32_t keys;
    while (1) {
        os_disp_fill(screen, 0, 0, DISP_WIDTH, DISP_HEIGHT, 0x0000);
        font_disp(screen, 0, 0, DISP_WIDTH, 0xFFFF, "Main menu", 64, CE_UTF8);
        font_disp(screen, 0, 16, DISP_WIDTH, 0xFFFF, "1. Playback", 64, CE_UTF8);
        font_disp(screen, 0, 32, DISP_WIDTH, 0xFFFF, "2. SW Trigger capture", 64, CE_UTF8);
        font_disp(screen, 0, 48, DISP_WIDTH, 0xFFFF, "3. HW Trigger capture", 64, CE_UTF8);
        font_disp(screen, 0, 64, DISP_WIDTH, 0xFFFF, "4. Manual capture", 64, CE_UTF8);
        os_disp_draw(screen);
        keys = app_wait_key();
        if (keys & KEY_MASK_1) {
            
        }
        else if (keys & KEY_MASK_2) {

        }
        else if (keys & KEY_MASK_3) {

        }
        else if (keys & KEY_MASK_4) {
            os_disp_fill(screen, 0, 0, DISP_WIDTH, DISP_HEIGHT, 0x0000);
            font_disp(screen, 0, 0, DISP_WIDTH, 0xFFFF, "Manual capture", 64, CE_UTF8);
            font_disp(screen, 0, 16, DISP_WIDTH, 0xFFFF, "Press 1 to start capturing, screen will turn off", 64, CE_UTF8);
            font_disp(screen, 0, 32, DISP_WIDTH, 0xFFFF, "Press 1 again to finish capturing.", 64, CE_UTF8);
            os_disp_draw(screen);
            uint32_t keys;
            do {
                keys = app_wait_key();
            } while (keys & KEY_MASK_1);
            os_disp_enter_sleep();
            os_camera_capture_manual_start();
            do {
                keys = app_wait_key();
            } while (keys & KEY_MASK_1);
            os_camera_capture_manual_stop();
            os_disp_exit_sleep();
            app_disp_captured_image();
            font_disp(screen, 0, 464, DISP_WIDTH, 0xFFFF, "Press any key to return.", 64, CE_UTF8);
            os_disp_draw(screen);
            app_wait_key();
        }
        else {
            font_disp(screen, 0, 464, DISP_WIDTH, 0xFFFF, "Invalid option", 64, CE_UTF8);
        }
    }
}