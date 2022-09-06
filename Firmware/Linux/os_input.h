//
// Project Fobu
// Copyright 2020 Wenting Zhang
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
// File : os_input.h
// Brief: Keypad input OS Abstraction Layer
//
#pragma once

#include <SDL.h>

#define KEY_MASK_LEFT   (1 << 0)
#define KEY_MASK_RIGHT  (1 << 1)
#define KEY_MASK_UP     (1 << 2)
#define KEY_MASK_DOWN   (1 << 3)
#define KEY_MASK_ENTER  (1 << 4)
#define KEY_MASK_1      (1 << 5)
#define KEY_MASK_2      (1 << 6)
#define KEY_MASK_3      (1 << 7)
#define KEY_MASK_4      (1 << 8)
#define KEY_MASK_REPLAY (1 << 9)
#define KEY_MASK_DELETE (1 << 10)

void os_input_init(void);
uint32_t os_input_get_keys(void);
void os_input_poll(void);
