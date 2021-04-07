//
// ringbuffer.c: A simple interrupt save ring buffer implementation
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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "ringbuf.h"

Ringbuf * ringbuf_init(int elements) {
    Ringbuf *buf = RINGBUF_MALLOC(sizeof(Ringbuf) +
            elements * sizeof(RINGBUF_ELEMENT_TYPE));
    assert(buf);
    buf->rx_ptr = 0;
    buf->tx_ptr = 0;
    buf->elements = elements;
    return buf;
}

void ringbuf_free(Ringbuf * buf) {
    RINGBUF_FREE(buf);
}

int ringbuf_getcount(Ringbuf *buf) {
    return buf->tx_ptr - buf->rx_ptr;
}

int ringbuf_getfree(Ringbuf *buf) {
    return buf->elements - (buf->tx_ptr - buf->rx_ptr);
}

bool ringbuf_push(Ringbuf *buf, RINGBUF_ELEMENT_TYPE *tx, int count) {
    if (ringbuf_getfree(buf) < count)
        return false;

    int tx_ptr_in_buf = buf->tx_ptr % buf->elements;
    int space_left = buf->elements - tx_ptr_in_buf;
    if (space_left > count) {
        memcpy(buf->buf + tx_ptr_in_buf, tx, count * RINGBUF_ELEMENT_SIZE);

    }
    else {
        memcpy(buf->buf + tx_ptr_in_buf, tx, space_left * RINGBUF_ELEMENT_SIZE);
        memcpy(buf->buf, tx + space_left, (count - space_left) * RINGBUF_ELEMENT_SIZE);
    }
    buf->tx_ptr += count;

    return true;
}

int ringbuf_pop(Ringbuf *buf, RINGBUF_ELEMENT_TYPE *rx, int count, bool required) {
    if (count == 0) {
        count = ringbuf_getcount(buf);
        if (count == 0)
            return 0;
    }
    else {
        int avail = ringbuf_getcount(buf);
        if (avail < count) {
            if (required)
                return false;
            else
                count = avail;
        }
    }

    int rx_ptr_in_buf = buf->rx_ptr % buf->elements;
    int space_left = buf->elements - rx_ptr_in_buf;
    if (space_left > count) {
        memcpy(rx, buf->buf + rx_ptr_in_buf, count * RINGBUF_ELEMENT_SIZE);
    }
    else {
        memcpy(rx, buf->buf + rx_ptr_in_buf, space_left * RINGBUF_ELEMENT_SIZE);
        memcpy(rx + space_left, buf->buf, (count - space_left) * RINGBUF_ELEMENT_SIZE);
    }
    buf->rx_ptr += count;

    return count;
}
