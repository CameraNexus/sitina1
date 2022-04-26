//
// ringbuffer.h: A simple interrupt safe ring buffer implementation
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

// Todo: use umm memory allocator
// Configurations
#define RINGBUF_ELEMENT_TYPE char   // Set the element type
#define RINGBUF_MALLOC malloc       // Optionally use other memory allocator
#define RINGBUF_FREE free

// Defines
#define RINGBUF_ELEMENT_SIZE sizeof(RINGBUF_ELEMENT_TYPE)

typedef struct {
    int rx_ptr;
    int tx_ptr;
    int elements;
    RINGBUF_ELEMENT_TYPE buf[];
} Ringbuf;

Ringbuf * ringbuf_init(int elements);
// Push count number of elements from tx into the ringbuf.
bool ringbuf_push(Ringbuf *buf, RINGBUF_ELEMENT_TYPE *tx, int count);
// Pop count number of elements from the ringbuf to rx.
// Count: count of the elements that should be popped out. Set to 0 for max
// Required: True: fail if not enough elements in buf. False: pop max possible.
// Return the actual number popped.
int ringbuf_pop(Ringbuf *buf, RINGBUF_ELEMENT_TYPE *rx, int count, bool required);
