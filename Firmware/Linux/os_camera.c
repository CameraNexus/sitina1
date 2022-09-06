//
// Project Sitina
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
#include <assert.h>
#include "os_camera.h"
#include "ccd_timing.h"

static uint16_t csi_buffer[CCD_FIELD_LINES * CCD_LINE_LENGTH];

void os_camera_init() {
    FILE *fp;
    fp = fopen("CAPTURE.BIN", "rb");
    assert(fp);
    fread(csi_buffer, CCD_FIELD_LINES * CCD_LINE_LENGTH * 2, 1, fp);
    fclose(fp);
}

void os_camera_capture_sw_sync(uint32_t integration_time) {
    // This function starts a capture with defined integration time
}

void os_camera_capture_hw_sync() {

}

void os_camera_capture_manual_start() {

}

void os_camera_capture_manual_stop() {
    // This function stops the integration and starts transferring
}

uint16_t *os_camera_get_buffer() {
    return csi_buffer;
}