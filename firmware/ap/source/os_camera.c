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
#include "fsl_cache.h"
#include "afe.h"
#include "csi.h"
#include "os_camera.h"
#include "os_display.h"
#include "ccd_timing.h"

// Reserve 4MB for LCD
uint16_t *camera_buffer_0 = (uint16_t *)0x80400000;
uint16_t *camera_buffer_1 = (uint16_t *)0x82000000;

static size_t image_size;
static uint32_t shutter_lines;

void os_cam_init(void) {
    afe_init();
    csi_init();
    image_size = 2*1024*1024;

    csi_submit_empty_buffer(camera_buffer_0);
    csi_submit_empty_buffer(camera_buffer_1);
}

void os_cam_deinit(void) {

}

void os_cam_set_capture_mode(CAM_CAPTURE_MODE cm) {

}

void os_cam_set_shutter_speed(uint32_t shutter_ns) {
    // Save the setting
    // 4.2G ns = 4.2M us = 4.2K ms = 4.2 s
    float shutter_lines_f = (float)shutter_ns / CCD_PRV_LINE_LENGTH / 41.667f;
    shutter_lines = (uint32_t)(shutter_lines_f + 0.5f);
    // Set draft speed
    afe_set_draft_shutter_speed(shutter_lines);
}

void os_cam_set_gain(uint32_t gain_x10) {

}

void os_cam_start(void) {
    csi_start();
    afe_start();
}

void os_cam_stop(void) {
    csi_stop();
    afe_stop();
}

void os_cam_submit_empty_buffer(uint16_t *buf) {
    csi_submit_empty_buffer((uint8_t *)buf);
}

uint16_t *os_cam_get_full_buffer() {
    uint16_t *buf = (uint16_t *)csi_get_full_buffer();
    if (buf) {
        DCACHE_InvalidateByRange((uint32_t)buf, image_size);
        return buf;
    }
    return NULL;
}

uint16_t *os_cam_still_capture(void) {
    // Disable display
    os_disp_enter_sleep();
    // Setup field
    csi_stop();
    csi_switch_resolution(CCD_LINE_LENGTH * 2, CCD_LINES);
    csi_start();
    afe_switch_to_still();
    // Once started, start capture
    // Wait till finished...
    uint16_t *buf;
    while ((buf = os_cam_get_full_buffer()) == NULL);
    // Pause AFE to save some power here
    afe_pause();
    // Restore settings
    csi_stop();
    csi_switch_resolution(CCD_PRV_LINE_LENGTH * 2, CCD_PRV_FIELD_LINES);
    afe_switch_to_draft();
    // Re-enable display
    os_disp_exit_sleep();
    return buf;
}

size_t os_cam_get_still_size(void) {
    return CCD_LINES * CCD_LINE_LENGTH * 2 * 2;
}
