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
#include <assert.h>
#include <SDL.h>
#include "os_camera.h"

#define FAKECAM_FILE "./fakecamcontent.bin"

#define PREVIEW_READOUT_TIME_MS 35
#define STILL_READOUT_TIME_MS   200

static uint8_t *cam_buf; // Fake camera source
static CAM_CAPTURE_MODE cam_cm;
static uint32_t cam_shutter_ms;
static SDL_TimerID timer_id = 0;
volatile bool frame_ready;

void os_cam_init(void) {
    // Load source of fake camera image
    FILE *fp;
    fp = fopen(FAKECAM_FILE, "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    fseek(fp, 0, SEEK_SET);
    cam_buf = malloc(size);
    assert(cam_buf);
    fread(cam_buf, 1, size, fp);
    fclose(fp);
    frame_ready = false;
}

void os_cam_deinit(void) {
    free(cam_buf);
}

void os_cam_set_capture_mode(CAM_CAPTURE_MODE cm) {
    cam_cm = cm;
}

void os_cam_set_shutter_speed(uint32_t shutter_ns) {
    cam_shutter_ms = shutter_ns / 1000;
}

void os_cam_set_gain(uint32_t gain_x10) {
    // ignore for now
}

static uint32_t get_total_readout_time() {
    if (cam_cm == CM_PREVIEW) {
        return PREVIEW_READOUT_TIME_MS + cam_shutter_ms;
    }
    else {
        return STILL_READOUT_TIME_MS + cam_shutter_ms;
    }
}

static uint32_t cam_timer_callback(uint32_t interval, void *name) {
    printf("Camera tick!\n");
    frame_ready = true;
    return get_total_readout_time();
}

void os_cam_start(void) {
    frame_ready = false;
    timer_id = SDL_AddTimer(get_total_readout_time(), cam_timer_callback, NULL);
}

void os_cam_stop(void) {
    if (timer_id)
        SDL_RemoveTimer(timer_id);
}

void os_cam_submit_empty_buffer(uint8_t *buf) {
    // Nothing
}

uint8_t *os_cam_get_full_buffer() {
    if (frame_ready) {
        frame_ready = false;
        return cam_buf;
    }
    else {
        return NULL;
    }
}
