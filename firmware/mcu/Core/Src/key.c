//
// Sitina 1
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
// File : key.c
// Brief: Key scan
//
#include "stm32g0xx_hal.h"
#include "key.h"

static uint16_t buttons = 0;
static int8_t rotenc = 0;
static uint8_t rotenc_raw_last = 0xff;

void key_init(void) {
    buttons = 0;
    rotenc = 0;
    rotenc_raw_last = 0xff;
}

void key_scan(uint16_t val) {
    buttons = val;
    // Some fixes
    if ((buttons & 0x0300) == 0x0000)
        buttons |= 0x0100;

    uint8_t rotenc_raw = (buttons >> 5) & 0x3;
    buttons |= 0x0060; // mask out rotenc
    buttons = ~buttons;

    if (rotenc_raw_last == 0xff) {
        // No previous value, ignore update
        rotenc_raw_last = rotenc_raw;
    }
    else {
        if ((rotenc_raw_last == 0x2) && (rotenc_raw == 0x0))
            rotenc++;
        else if ((rotenc_raw_last == 0x0) && (rotenc_raw == 0x2))
            rotenc--;
        rotenc_raw_last = rotenc_raw;
    }
}

uint16_t key_get_buttons() {
    return buttons;
}

int8_t key_get_rotenc() {
    return rotenc;
}

void key_set_rotenc(int8_t val) {
    rotenc = val;
}
