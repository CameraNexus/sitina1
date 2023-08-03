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
#pragma once

typedef enum {
    CM_PREVIEW, // preview size image
    CM_STILL // full size still image
} CAM_CAPTURE_MODE;

void os_cam_init(void);
void os_cam_deinit(void);
void os_cam_set_capture_mode(CAM_CAPTURE_MODE cm);
void os_cam_set_shutter_speed(uint32_t shutter_ns);
void os_cam_set_gain(uint32_t gain_x10);
void os_cam_start(void);
void os_cam_stop(void);
void os_cam_submit_empty_buffer(uint8_t *buf);
uint8_t os_cam_get_full_buffer();
