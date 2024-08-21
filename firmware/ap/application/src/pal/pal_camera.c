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

}

void pal_cam_set_shutter_speed(uint32_t shutter_ns) {

}

void pal_cam_set_gain(uint32_t gain_x10) {

}

void pal_cam_start(void) {
    //afe_start();
    dcif_engage();
    ccdtg_start();
}

void pal_cam_stop(void) {

}

uint16_t *pal_cam_get_full_buffer() {
    return (uint16_t *)dcif_waitnextbuffer();
}

uint16_t *pal_cam_still_capture(void) {

}
