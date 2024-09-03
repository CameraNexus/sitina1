//
// Sitina1
// Copyright 2024 Wenting Zhang
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

#include <stdint.h>

// Define meta data
typedef struct {
    uint16_t val; // ISO speed
    uint16_t cdsgain; // 0 = -3dB, 4 = 0dB, 6 = +3dB, 7 = +6dB
    uint16_t vgagain; // 6-42dB in 0.0358dB steps
    char name[6];
} ISO_SETPOINT;

typedef struct {
    uint32_t time; // time in us
    uint32_t draft_lines; // timing in draft mode // TBD
    char name[8];
} SHUT_SETPOINT;

typedef enum {
    AEM_P, // Auto
    AEM_A, // Aperature priority
    AEM_S, // Shutter speed priority
    AEM_M, // Manual
} AE_MODE;

extern const ISO_SETPOINT iso_setpoints[];
extern const int no_iso_setpoints;
extern const SHUT_SETPOINT shut_setpoints[];
extern const int no_shut_setpoints;

extern int current_shutter;
extern int current_iso;
extern AE_MODE current_aem;

extern uint8_t histogram[256];
