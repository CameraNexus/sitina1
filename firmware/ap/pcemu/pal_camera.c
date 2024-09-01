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
#include <SDL.h>
#include "pal_camera.h"

#define CAM_STILL_SAMPLE_FILE   "./cam_still_sample.bin"
#define CAM_DRAFT_SAMPLE_FILE   "./cam_draft_sample.bin"

#define PREVIEW_READOUT_TIME_MS 35
#define STILL_READOUT_TIME_MS   200

// Emulated camera image source
static uint8_t *cam_buf;
static uint8_t *cam_still_buf;
static uint8_t *cam_draft_buf;
static CAM_CAPTURE_MODE cam_cm;
static uint32_t cam_shutter_ms;
static SDL_TimerID timer_id = 0;
volatile bool frame_ready;

static uint8_t *load_file(char *fn) {
    uint8_t *buf;
    FILE *fp;
    fp = fopen(fn, "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = (size_t)ftell(fp);
    printf("Loading file %d bytes\n", size);
    fseek(fp, 0, SEEK_SET);
    buf = malloc(size);
    assert(buf);
    fread(buf, 1, size, fp);
    fclose(fp);
    return buf;
}

void pal_cam_init(void) {
    // Load source of fake camera image
    cam_still_buf = load_file(CAM_STILL_SAMPLE_FILE);
    cam_draft_buf = load_file(CAM_DRAFT_SAMPLE_FILE);
    cam_buf = cam_draft_buf;
    frame_ready = false;
}

void pal_cam_deinit(void) {
    free(cam_still_buf);
    free(cam_draft_buf);
}

void pal_cam_set_capture_mode(CAM_CAPTURE_MODE cm) {
    cam_cm = cm;
    if (cm == CM_DRAFT) {
        cam_buf = cam_draft_buf;
    }
    else {
        cam_buf = cam_still_buf;
    }
}

void pal_cam_set_preview_shutter_speed(uint32_t shutter_line) {

}

void pal_cam_set_still_shutter_speed(uint32_t shutter_us) {

}

void pal_cam_set_gain(uint32_t cds_gain, uint32_t vga_gain) {
    // ignore for now
}

static uint32_t get_total_readout_time() {
    if (cam_cm == CM_DRAFT) {
        return PREVIEW_READOUT_TIME_MS + cam_shutter_ms;
    }
    else {
        return STILL_READOUT_TIME_MS + cam_shutter_ms;
    }
}

static uint32_t cam_timer_callback(uint32_t interval, void *name) {
    frame_ready = true;
    return get_total_readout_time();
}

void pal_cam_start(void) {
    frame_ready = false;
    //frame_ready = true;
    timer_id = SDL_AddTimer(get_total_readout_time(), cam_timer_callback, NULL);
}

void pal_cam_stop(void) {
    if (timer_id)
        SDL_RemoveTimer(timer_id);
}

void pal_cam_submit_empty_buffer(uint16_t *buf) {
    // Nothing
}

uint16_t *pal_cam_get_full_buffer() {
    if (frame_ready) {
        frame_ready = false;
        return (uint16_t *)cam_buf;
    }
    else {
        return NULL;
    }
}

uint16_t *pal_cam_still_capture(void) {
    SDL_Delay(300);
    return (uint16_t *)cam_still_buf;
}

size_t pal_cam_get_still_size(void) {
    //return CCD_LINES * CCD_LINE_LENGTH * 2 * 2;
    return 0; // TODO
}
