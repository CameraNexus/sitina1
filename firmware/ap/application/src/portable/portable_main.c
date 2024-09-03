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
#include "pal.h"
#include "pal_camera.h"
#include "pal_display.h"
#include "pal_filesystem.h"
#include "pal_input.h"
#include "pal_power.h"
#include "image_processing.h"
#include "uilib.h"
#include "aondisp.h"
#include "gui.h"
#include "metadata.h"

#include "dcif.h" // TODO: Remove this, saved RAW file shouldn't require this

uint32_t previewbuf[PROC_PREVIEW_HEIGHT * PROC_PREVIEW_WIDTH];

uint32_t get_save_seq(void) {
    int seq = 0;
    File *fp = pal_fs_open("SEQ.TXT", OM_READ);
    if (fp) {
        int fsize = pal_fs_size(fp);
        char buf[7] = "0";
        if (fsize < 6) {
            pal_fs_read(fp, buf, fsize);
            buf[fsize] = '\0';
            seq = atoi(buf);
        }
        pal_fs_close(fp);
    }
    return seq;
}

void update_save_seq(uint32_t seq) {
    File *fp = pal_fs_open("SEQ.TXT", OM_WRITE);
    char buf[7];
    snprintf(buf, 7, "%d", seq);
    pal_fs_write(fp, buf, strlen(buf));
    pal_fs_close(fp);
}

void portable_main(void) {
    int result;

    pal_init();
    pal_pwr_init();
    pal_cam_init();
    pal_disp_init();
    pal_input_init();

    uilib_init();
    ip_init();
    gui_init();

    // aondisp_clear(0x00);
    // aondisp_set_font(&font_12x20);
    // aondisp_draw_string(0, 0, "RUNNING", 10, 1);
    // aondisp_update();

    result = pal_fs_init();
    if (result == FS_DISK_ERR) {
        gui_show_fatal("Unable to initialize SD card");
    }
    else if (result == FS_INVALID_FS) {
        gui_show_fatal("Unable to find valid filesystem");
    }
    else if (result != FS_OK) {
        gui_show_fatal("Filesystem initialization failed\n");
    }

    uilib_mark_update();

    pal_cam_start();

    bool redraw = true;
    uint32_t seq = get_save_seq();

    while(1) {
        uint32_t *disp_buf = pal_disp_get_buffer();
        uilib_set_framebuffer((uint8_t *)disp_buf);

        uint16_t *cambuf = pal_cam_get_full_buffer();

        CAP_ACT act = gui_run_capture_screen(redraw);
        redraw = false;

        if (act == CAP_ACT_CAPTURE) {
            // Release shutter and save image
            //pal_cam_set_capture_mode(CM_STILL);
            pal_cam_set_still_shutter_speed(shut_setpoints[current_shutter].time);
            // Turn off LCD
            memset(disp_buf, 0, 480*480*4);
            pal_disp_return_buffer(disp_buf);
            //pal_disp_enter_sleep();
            // Discard the current frame
            cambuf = pal_cam_get_full_buffer();
            // Get the full frame
            cambuf = pal_cam_get_full_buffer();
            // Turn ON LCD and save image
            //pal_disp_exit_sleep();
            // TODO: Proper UI here
            uilib_draw_string(0, 0, 480, "Saving", 8, false, COLOR_WHITE, COLOR_BLACK);

            ip_filter_draft_image(cambuf, previewbuf, histogram);
            uint32_t *wrptr = disp_buf + 480 * 80;
            for (int y = 8; y < 328; y++) {
                for (int x = 0; x < 480; x++) {
                    uint32_t pix = previewbuf[y * PROC_DRAFT_WIDTH + 479 - x];
                    *wrptr++ = pix;
                }
            }
            pal_disp_return_buffer(disp_buf);
            // Save to SD card
            char fn[13];
            snprintf(fn, 13, "SNE%05d.RAW", seq);
            File *fp = pal_fs_open(fn, OM_WRITE);
            size_t filesize = CAM_BUFSIZE;
            uint8_t *saveptr = (uint8_t *)cambuf;
            const size_t chunksize = 1048576;
            for (int i = 0; i < (filesize / chunksize); i++) {
                pal_fs_write(fp, saveptr, chunksize);
                saveptr += chunksize;
                filesize -= chunksize;
                //gui_set_progress(i * chunksize * 100 / filesize);
                //os_disp_return_buffer(disp_buf);
            }
            if (filesize != 0) {
                pal_fs_write(fp, saveptr, filesize);
                //gui_set_progress(100);
                //os_disp_return_buffer(disp_buf);
            }
            pal_fs_close(fp);
            seq++;
            update_save_seq(seq);
            // Resume live view
            pal_cam_set_capture_mode(CM_DRAFT);
            redraw = true;
        }

        ip_filter_draft_image(cambuf, previewbuf, histogram);

        uint32_t *wrptr = disp_buf + 480 * 80;
        for (int y = 8; y < 328; y++) {
            for (int x = 0; x < 480; x++) {
                uint32_t pix = previewbuf[y * PROC_DRAFT_WIDTH + 479 - x];
                *wrptr++ = pix;
            }
        }

        pal_input_scan();
        pal_disp_return_buffer(disp_buf);
    }

    //gui_deinit();

    pal_disp_deinit();
    pal_fs_deinit();
    pal_input_deinit();
}
