//
// Sitina1
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
#include <stdio.h>
#include "os_display.h"
#include "os_filesystem.h"
#include "os_input.h"
#include "lvgl.h"

#define LVGL_SCREEN_BUF_HEIGHT DISP_HEIGHT

lv_indev_t * indev_keypad;

static lv_disp_draw_buf_t draw_buf_dsc_1;
static lv_color_t buf[DISP_WIDTH * LVGL_SCREEN_BUF_HEIGHT];
static lv_disp_drv_t disp_drv;

// Flush the content of the internal buffer the specific area on the display
// You can use DMA or any hardware acceleration to do this operation in the
// background but 'lv_disp_flush_ready()' has to be called when finished.
static void disp_flush(lv_disp_drv_t * disp_drv, const lv_area_t * area,
        lv_color_t * color_p) {
    uint32_t *screen = os_disp_get_buffer();
    uint32_t *rdptr = (uint32_t *)color_p;
    for (int y = area->y1; y <= area->y2; y++) {
        int width = area->x2 - area->x1 + 1;
        memcpy(screen + y * DISP_WIDTH + area->x1, rdptr, width * 4);
        rdptr += width;
    }
    os_disp_return_buffer(screen);
    lv_disp_flush_ready(disp_drv);
}

void lv_port_disp_init(void) {
    lv_disp_draw_buf_init(&draw_buf_dsc_1, buf, NULL, DISP_WIDTH * LVGL_SCREEN_BUF_HEIGHT);

    // Register the display in LVGL
    lv_disp_drv_init(&disp_drv);       // Basic initialization

    // Set up the functions to access to your display

    // Set the resolution of the display
    disp_drv.hor_res = DISP_WIDTH;
    disp_drv.ver_res = DISP_HEIGHT;

    // Used to copy the buffer's content to the display
    disp_drv.flush_cb = disp_flush;

    // Set a display buffer
    disp_drv.draw_buf = &draw_buf_dsc_1;

#if LV_USE_GPU
    // Optionally add functions to access the GPU. 
    // (Only in buffered mode, LV_VDB_SIZE != 0)
#error "GPU support is not supported yet"

    // Blend two color array using opacity
    disp_drv.gpu_blend_cb = gpu_blend;

    // Fill a memory array with a color
    disp_drv.gpu_fill_cb = gpu_fill;
#endif

    // Finally register the driver
    lv_disp_drv_register(&disp_drv);
}

// Get the currently being pressed key. 0 if no key is pressed
static uint32_t keypad_get_key(void) {
    uint32_t key_mask = os_input_get_keys();

    if (key_mask & KEY_MASK_UP)
        return LV_KEY_UP;
    else if (key_mask & KEY_MASK_DOWN)
        return LV_KEY_DOWN;
    else if (key_mask & KEY_MASK_LEFT)
        return LV_KEY_LEFT;
    else if (key_mask & KEY_MASK_RIGHT)
        return LV_KEY_RIGHT;
    else if (key_mask & KEY_MASK_YES)
        return LV_KEY_ENTER;
    else if (key_mask & KEY_MASK_NO)
        return LV_KEY_ESC;

    // Start and select will be treated as soft buttons, not part of keypad

    return 0;
}

// Will be called by the library to read the keypad
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data) {
    static uint32_t last_key = 0;

    // Get whether the a key is pressed and save the pressed key
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;
        last_key = act_key;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

void lv_port_indev_init(void) {
    lv_indev_drv_t indev_drv;

    // Register a keypad input device
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv.read_cb = keypad_read;
    indev_keypad = lv_indev_drv_register(&indev_drv);
}
