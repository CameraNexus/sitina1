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
#include <stdbool.h>
#include "os_display.h"
#include "os_filesystem.h"
#include "os_input.h"
#include "os_timer.h"
#include "lvgl.h"
#include "lv_port.h"
#include "demos/lv_demos.h"

void app_tick(void) {
    lv_tick_inc(10);
}

void app_main(void) {
    os_disp_init();
    os_fs_init();
    os_input_init();
    os_timer_register_systick(10, app_tick);

    lv_init();

    lv_port_disp_init();
    lv_port_indev_init();

    lv_demo_widgets();

    uint32_t lvgl_timeout = 0;
    while(1) {
        os_input_scan();
        lvgl_timeout = lv_timer_handler_run_in_period(lvgl_timeout);
    }

    os_timer_deinit();
    os_disp_deinit();
    os_fs_deinit();
    os_input_deinit();
}