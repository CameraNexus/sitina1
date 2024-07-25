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
// File : key.h
// Brief: Key scan
//
#pragma once

#define KEY_FN1_MASK        0x0001
#define KEY_FN2_MASK        0x0002
#define KEY_FN3_MASK        0x0004
#define KEY_FN4_MASK        0x0008
#define KEY_SHUTTER_MASK    0x0010
#define KEY_UP_MASK         0x0100
#define KEY_LEFT_MASK       0x0200
#define KEY_RIGHT_MASK      0x0400
#define KEY_DOWN_MASK       0x0800
#define KEY_FN5_MASK        0x1000
#define KEY_FN6_MASK        0x2000

void key_init(void);
void key_scan(uint16_t val); // Called from LCD driver due to shared SPI
uint16_t key_get_buttons();
int8_t key_get_rotenc();
void key_set_rotenc(int8_t val);
