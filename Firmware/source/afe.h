//
// afe.h: AD9990 AFE driver
//
// Copyright 2021 Wenting Zhang <zephray@outlook.com>
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

#define AFE_MOSI_GPIO       GPIO10
#define AFE_MOSI_GPIO_PIN   2
#define AFE_SCK_GPIO        GPIO10
#define AFE_SCK_GPIO_PIN    0
#define AFE_CS_GPIO         GPIO9
#define AFE_CS_GPIO_PIN     30
#define AFE_RST_GPIO        GPIO8
#define AFE_RST_GPIO_PIN    31
#define AFE_SYNC_GPIO       GPIO9
#define AFE_SYNC_GPIO_PIN   0
#define AFE_STROBE_GPIO     GPIO9
#define AFE_STROBE_GPIO_PIN 1

void afe_init(void);
void afe_start(void);
void afe_stop(void);

