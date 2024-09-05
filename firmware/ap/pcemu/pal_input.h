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

#define KEY_MASK_LEFT       0x0200
#define KEY_MASK_RIGHT      0x0400
#define KEY_MASK_UP         0x0100
#define KEY_MASK_DOWN       0x0800
#define KEY_MASK_SHUTTER    0x0010
#define KEY_MASK_FN1        0x0001
#define KEY_MASK_FN2        0x0002
#define KEY_MASK_FN3        0x0004
#define KEY_MASK_FN4        0x0008
#define KEY_MASK_FN5        0x1000
#define KEY_MASK_FN6        0x2000
#define KEY_MASK_FN7        0x4000
#define KEY_MASK_FN8        0x8000
#define KEY_MASK_PEK_SHORT  0x010000
#define KEY_MASK_PEK_LONG   0x020000

void pal_input_init(void);
void pal_input_deinit(void);
uint32_t pal_input_get_keys(void);
int pal_input_get_encoder(uint32_t id);
void pal_input_scan(void);
