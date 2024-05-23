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
#pragma once

void axiep_reset();
void axiep_apply(uint8_t awid, uint32_t awaddr, uint8_t awlen, uint8_t awsize,
        uint8_t awburst, uint8_t awvalid, uint8_t &awready, uint64_t wdata,
        uint8_t wstrb, uint8_t wlast, uint8_t wvalid, uint8_t &wready,
        uint8_t bready, uint8_t &bid, uint8_t &bresp, uint8_t &bvalid,
        uint8_t arid, uint32_t araddr, uint8_t arlen, uint8_t arsize,
        uint8_t arburst, uint8_t arvalid, uint8_t &arready, uint8_t rready,
        uint8_t &rid, uint64_t &rdata, uint8_t &rresp, uint8_t &rlast, 
        uint8_t &rvalid);
