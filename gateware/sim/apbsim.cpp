//
// Sitina simulator
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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <queue>
#include "apbsim.h"

// Addr and data are packed into a single 64-bit request word
// Addr LSB is used to indicate the type (1-read/ 0-write)
static std::queue<uint64_t> req_buf;
static std::queue<uint32_t> resp_buf;

static bool in_transfer;
static bool is_read;

void apbsim_reset() {
    in_transfer = false;
    is_read = false;
}

void apbsim_apply(uint8_t &pwrite, uint32_t &pwdata, uint32_t &paddr,
        uint8_t &penable, uint8_t &psel, uint8_t pready, uint32_t prdata) {
    if (in_transfer) {
        // Check finish
        if (pready) {
            penable = 0;
            psel = 0;
            pwrite = 0;
            pwdata = 0;
            in_transfer = false;
            if (is_read) {
                resp_buf.push(prdata);
            }
        }
    }
    else {
        if (req_buf.empty()) {
            // Nothing to do
            pwrite = 0;
            pwdata = 0;
            paddr = 0;
            penable = 0;
            psel = 0;
        }
        else {
            // Has data
            uint64_t next_req = req_buf.front();
            req_buf.pop();
            uint32_t data = (uint32_t)(next_req & 0xffffffffull);
            uint32_t addr = (uint32_t)(next_req >> 32);
            if (addr & 0x1) {
                // Read request
                pwrite = 0;
                pwdata = 0;
                is_read = true;
            }
            else {
                // Write request
                pwrite = 1;
                pwdata = data;
                is_read = false;
            }
            paddr = addr & 0xfffffffeul;
            penable = 1;
            psel = 1;
            in_transfer = true;
        }
    }
}

// Testbench functions
void apbsim_write(uint32_t addr, uint32_t val) {
    uint64_t req = (uint64_t)addr << 32 | val;
    req_buf.push(req);
}

void apbsim_read(uint32_t addr) {
    uint64_t req = (uint64_t)(addr | 0x1ul) << 32;
    req_buf.push(req);
}

bool apbsim_get_read_val(uint32_t &val) {
    if (resp_buf.size() > 0) {
        val = resp_buf.front();
        resp_buf.pop();
        return true;
    }
    return false;
}

bool apbsim_is_busy(void) {
    return in_transfer || (req_buf.size() > 0);
}