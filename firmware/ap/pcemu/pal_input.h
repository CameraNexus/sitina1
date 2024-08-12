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

#define KEY_MASK_LEFT   (1 << 0)
#define KEY_MASK_RIGHT  (1 << 1)
#define KEY_MASK_UP     (1 << 2)
#define KEY_MASK_DOWN   (1 << 3)
#define KEY_MASK_FN1    (1 << 4)
#define KEY_MASK_FN2    (1 << 5)
#define KEY_MASK_FN3    (1 << 6)
#define KEY_MASK_FN4    (1 << 7)
#define KEY_MASK_FN5    (1 << 8)
#define KEY_MASK_FN6    (1 << 9)

void pal_input_init(void);
void pal_input_deinit(void);
uint32_t pal_input_get_keys(void);
int pal_input_get_encoder(uint32_t id);
void pal_input_scan(void);
