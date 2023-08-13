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
#include "os_camera.h"
#include "os_display.h"
#include "os_filesystem.h"
#include "os_input.h"
#include "os_timer.h"
#include "gui.h"
#include "image_processing.h"

volatile bool shutter_requested = false;

void shutter_release(void) {
    shutter_requested = true;
}

uint32_t get_save_seq(void) {
    File *fp = os_fs_open("SEQ.TXT", OM_WRITE);
    int fsize = os_fs_size(fp);
    int seq = 0;
    char buf[7] = "0";
    if (fsize < 6) {
        os_fs_read(fp, buf, fsize);
        buf[fsize] = '\0';
        seq = atoi(buf);
    }
    else {
        // Invalid, reset
        os_fs_write(fp, buf, 1);
    }
    os_fs_close(fp);
    return seq;
}

void update_save_seq(uint32_t seq) {
    File *fp = os_fs_open("SEQ.TXT", OM_WRITE);
    char buf[7];
    snprintf(buf, 7, "%d", seq);
    os_fs_write(fp, buf, strlen(buf));
    os_fs_close(fp);
}

void app_main(void) {
    os_disp_init();
    os_fs_init();
    os_input_init();
    //os_timer_register_systick(10, app_tick);
    os_cam_init();
    os_cam_set_capture_mode(CM_DRAFT);

    gui_init();
    gui_setup_preview_screen();
    gui_setup_progress_screen();
    gui_show_preview_screen();

    uint32_t *disp_buf = os_disp_get_buffer();

    uint32_t seq = get_save_seq();

    os_cam_start();

    shutter_requested = false;

    uint16_t *cam_buf;
    while(1) {
        while ((cam_buf = os_cam_get_full_buffer()) == NULL);
        //uint32_t duration = OSA_TimeGetMsec();
        ip_filter_draft_image(cam_buf, disp_buf + 120*720, 0);
        os_cam_submit_empty_buffer(cam_buf);
        //duration = OSA_TimeGetMsec() - duration;
        //printf("%d ms\n", duration);
        os_input_scan();
        gui_scan_input();

        if (shutter_requested) {
            gui_hide_preview_screen();
            // Start still capture
            cam_buf = os_cam_still_capture();
            gui_show_progress_screen();
            gui_set_progress(0);
            ip_filter_still_image(cam_buf, disp_buf + 120*720, 0);
            os_disp_return_buffer(disp_buf);
            // Save image here...
            char fn[13];
            snprintf(fn, 13, "SNE%05d.RAW", seq);
            File *fp = os_fs_open(fn, OM_WRITE);
            size_t filesize = os_cam_get_still_size();
            uint8_t *wrptr = (uint8_t *)cam_buf;
            const size_t chunksize = 1048576;
            for (int i = 0; i < (filesize / chunksize); i++) {
                os_fs_write(fp, wrptr, chunksize);
                wrptr += chunksize;
                filesize -= chunksize;
                gui_set_progress(i * chunksize * 100 / filesize);
                os_disp_return_buffer(disp_buf);
            }
            if (filesize != 0) {
                os_fs_write(fp, wrptr, filesize);
                gui_set_progress(100);
                os_disp_return_buffer(disp_buf);
            }
            os_fs_close(fp);
            seq++;
            update_save_seq(seq);
            gui_hide_progress_screen();
            gui_show_preview_screen();
            shutter_requested = false;
            os_cam_submit_empty_buffer(cam_buf);
            os_cam_start();
        }
        os_disp_return_buffer(disp_buf);
    }

    gui_deinit();

    //os_timer_deinit();
    os_disp_deinit();
    os_fs_deinit();
    os_input_deinit();
}
