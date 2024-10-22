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
#include <stdbool.h>
#include <stdint.h>
#include "mcusvc.h"
#include "pal_input.h"

uint32_t key_state;
uint32_t key_used;
int encoder_count;

void pal_input_init(void) {
    key_state = 0;
    key_used = 0;
    encoder_count = 0;
}

void pal_input_deinit(void) {

}

uint32_t pal_input_get_keys(void) {
#if 0
    uint32_t retval = key_state & ~key_used; // only return newly pressed keys
    key_used |= key_state;
    return retval;
#else
    return key_state;
#endif
}

int pal_input_get_encoder(uint32_t id) {
    if (id != 0)
        return 0;
    int cnt = encoder_count;
    encoder_count = 0;
    return cnt;
}

void pal_input_scan(void) {
    key_state = mcusvc_get_buttons();
    encoder_count += mcusvc_get_rotenc();
    // clear keys that have been released
    key_used &= key_state;
}
