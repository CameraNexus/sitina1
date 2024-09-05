//
// Sitina1
// Copyright 2023 Wenting Zhang
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
#include <assert.h>
#include "afe.h"
#include "dcif.h"
#include "ccdtg.h"
#include "pal_camera.h"

void pal_cam_init(void) {
    afe_init();
    dcif_init();
    ccdtg_init();
}

void pal_cam_deinit(void) {

}

void pal_cam_set_capture_mode(CAM_CAPTURE_MODE cm) {
    if (cm == CM_DRAFT) {
        ccdtg_set_line_skip(2); // Enable line skipping
        ccdtg_set_eshut(EMBED_ESHUT);
        dcif_set_draft();
    }
    else if (cm == CM_STILL) {
        ccdtg_set_line_skip(0); // Disable line skipping
        ccdtg_set_eshut(START_ESHUT);
        dcif_set_still();
    }
}

void pal_cam_set_preview_shutter_speed(uint32_t shutter_line) {
    ccdtg_set_embed_eshut_delay(shutter_line);
}

void pal_cam_set_still_shutter_speed(uint32_t shutter_us) {
    const uint32_t MIN_SHUTTER = 132; // Minimum time due to shift delay
    uint32_t htime;
    uint32_t vtime;
    if (shutter_us <= MIN_SHUTTER) {
        // Minimum delay time
        htime = 3;
        vtime = 0;
    }
    else if (shutter_us < 10000) {
        // 1us resolution, up to 10000us (10ms)
        htime = 27 * 4 - 1;
        vtime = shutter_us - MIN_SHUTTER;
    }
    else if (shutter_us < 6400 * 1000) {
        // 100us resolution, up to 3200ms (3.2s)
        // Has error of MIN_SHUTTER us (around 1% max)
        htime = 2700 * 4 - 1;
        vtime = shutter_us / 100;
    }
    else {
        // Not supported yet
        htime =  3;
        vtime = 0;
    }
    ccdtg_set_start_eshut_delay(htime, vtime);
}

void pal_cam_set_gain(uint32_t cds_gain, uint32_t vga_gain) {
    afe_set_gain(cds_gain, vga_gain);
}

void pal_cam_set_shl(uint8_t ch, uint8_t shd, uint8_t shp) {
    afe_write_reg(ch ? 0x138 : 0x238,
            (shd << 0) | // SHDLOC
            (shp << 8) | // SHPLOC
            (8 << 16)); // SHPWIDTH
}

void pal_cam_start(void) {
    //afe_start();
    dcif_engage();
    ccdtg_start();
}

void pal_cam_stop(void) {
    ccdtg_stop();
}

uint16_t *pal_cam_get_full_buffer() {
    return (uint16_t *)dcif_waitnextbuffer();
}

uint16_t *pal_cam_still_capture(void) {

}
