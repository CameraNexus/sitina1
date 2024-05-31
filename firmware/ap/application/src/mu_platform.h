//
// Copyright (c) 2024 Wenting Zhang
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

// Common definition
#define GPIO_PIN_0  0x00000001
#define GPIO_PIN_1  0x00000002
#define GPIO_PIN_2  0x00000004
#define GPIO_PIN_3  0x00000008
#define GPIO_PIN_4  0x00000010
#define GPIO_PIN_5  0x00000020
#define GPIO_PIN_6  0x00000040
#define GPIO_PIN_7  0x00000080
#define GPIO_PIN_8  0x00000100
#define GPIO_PIN_9  0x00000200
#define GPIO_PIN_10 0x00000400
#define GPIO_PIN_11 0x00000800
#define GPIO_PIN_12 0x00001000
#define GPIO_PIN_13 0x00002000
#define GPIO_PIN_14 0x00004000
#define GPIO_PIN_15 0x00008000
#define GPIO_PIN_16 0x00010000
#define GPIO_PIN_17 0x00020000
#define GPIO_PIN_18 0x00040000
#define GPIO_PIN_19 0x00080000
#define GPIO_PIN_20 0x00100000
#define GPIO_PIN_21 0x00200000
#define GPIO_PIN_22 0x00400000
#define GPIO_PIN_23 0x00800000
#define GPIO_PIN_24 0x01000000
#define GPIO_PIN_25 0x02000000
#define GPIO_PIN_26 0x04000000
#define GPIO_PIN_27 0x08000000
#define GPIO_PIN_28 0x10000000
#define GPIO_PIN_29 0x20000000
#define GPIO_PIN_30 0x40000000
#define GPIO_PIN_31 0x80000000

typedef struct {
    volatile uint32_t REG_ODR;
    volatile uint32_t REG_IDR;
    volatile uint32_t REG_BSR;
    volatile uint32_t REG_BCR;
    volatile uint32_t REG_OER;
} mu_gpio_inst_t;

// Platform specific definition
#define GPIO0_BASE  0x40000000

#define GPIO0_ODR   (volatile uint32_t *)(GPIO0_BASE + 0x00)
#define GPIO0_IDR   (volatile uint32_t *)(GPIO0_BASE + 0x04)
#define GPIO0_BSR   (volatile uint32_t *)(GPIO0_BASE + 0x08)
#define GPIO0_BCR   (volatile uint32_t *)(GPIO0_BASE + 0x0C)
#define GPIO0_OER   (volatile uint32_t *)(GPIO0_BASE + 0x10)
