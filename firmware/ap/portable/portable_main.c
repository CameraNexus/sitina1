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
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "os_camera.h"
#include "os_display.h"
#include "os_filesystem.h"
#include "os_input.h"
#include "os_timer.h"
#include "uilib.h"
#include "gui.h"

void portable_main(void) {
    os_disp_init();
    os_fs_init();
    os_input_init();
    //os_timer_register_systick(10, app_tick);
    //os_cam_init();
    //os_cam_set_capture_mode(CM_DRAFT);

    uilib_init();
    //gui_init();
    //gui_show_preview_screen();

    uint32_t *disp_buf = os_disp_get_buffer();

    //os_cam_start();
    uilib_mark_update();

    //uint16_t *cam_buf;
    while(1) {
        uilib_set_framebuffer((uint8_t *)disp_buf);
        
        gui_run_capture_screen(false);
        //while ((cam_buf = os_cam_get_full_buffer()) == NULL);
        //uint32_t duration = OSA_TimeGetMsec();
        //ip_filter_draft_image(cam_buf, disp_buf + 120*720, 0);
        //os_cam_submit_empty_buffer(cam_buf);
        os_input_scan();
        //gui_scan_input();
        os_disp_return_buffer(disp_buf);
    }

    //gui_deinit();

    //os_timer_deinit();
    os_disp_deinit();
    os_fs_deinit();
    os_input_deinit();
}
