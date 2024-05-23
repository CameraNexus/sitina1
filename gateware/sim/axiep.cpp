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
#include "memmap.h"
#include "axiep.h"

#define BURST_FIXED 0x0
#define BURST_INCR  0x1
#define BURST_WRAP  0x2
#define RESP_OKAY   0x0
#define RESP_EXOKAY 0x1
#define RESP_SLVERR 0x2
#define RESP_DECERR 0x3

// Global states
typedef struct {
    // Request
    uint64_t addr;
    uint8_t id;
    uint8_t size;
    uint8_t len;
    uint8_t burst;
    // State
    int beatcount;
    uint8_t firstbeat;
    int bubble;
} req_t;

// Current processing request
static req_t read_req;
static req_t write_req;

//#define VERBOSE

#define ASSERTS

static int get_beats(uint8_t size, uint8_t len) {
    int byte_size = 1 << size;
    int burst_len = len + 1;
    int total_len = byte_size * burst_len;
    int beats = (total_len + 7) / 8;
    return beats;
}

void axiep_reset() {
    // Called during simulator reset
    read_req.beatcount = 0;
    write_req.beatcount = 0;
}

void axiep_apply(uint8_t awid, uint32_t awaddr, uint8_t awlen, uint8_t awsize,
        uint8_t awburst, uint8_t awvalid, uint8_t &awready, uint64_t wdata,
        uint8_t wstrb, uint8_t wlast, uint8_t wvalid, uint8_t &wready,
        uint8_t bready, uint8_t &bid, uint8_t &bresp, uint8_t &bvalid,
        uint8_t arid, uint32_t araddr, uint8_t arlen, uint8_t arsize,
        uint8_t arburst, uint8_t arvalid, uint8_t &arready, uint8_t rready,
        uint8_t &rid, uint64_t &rdata, uint8_t &rresp, uint8_t &rlast, 
        uint8_t &rvalid) {
    // Called during every posedge clk
    // Default values
    awready = 1;
    arready = 1;
    wready = 0;
    bvalid = 0;
    rvalid = 0;

    // Handle read request
    if ((read_req.beatcount == 0) && (arvalid)) {
        read_req.addr = araddr;
        read_req.id = arid;
        read_req.size = arsize;
        read_req.len = arlen;
        read_req.burst = arburst;
        if (arburst != BURST_INCR) {
            fprintf(stderr, "Unsupported burst type %d", arburst);
        }
        #ifdef VERBOSE
        fprintf(stderr, "MEM: AR %08x SIZE %d LEN %d BURST %d ID %d\n",
                read_req.addr, read_req.size, read_req.len, read_req.burst, read_req.id);
        #endif
        read_req.beatcount = get_beats(arsize, arlen);
        read_req.firstbeat = 1;
        read_req.bubble = 0;
        arready = 0;
    }
    else if (read_req.beatcount != 0) {
        if ((!rready) && (read_req.firstbeat == 0)) {
            // Previous beat is not processed
            #ifdef VERBOSE
            fprintf(stderr, "Stall\n");
            #endif
            arready = 0;
            rvalid = 1;
        }
        else if ((read_req.beatcount == 1) && (read_req.firstbeat == 0)) {
            // End
            rvalid = 0;
            arready = 1;
            read_req.beatcount = 0;
        }
        else {
            arready = 0;
            rvalid = 1;
            rid = read_req.id;
            rdata = memmap_read(read_req.addr);
            rresp = RESP_OKAY;
            rlast = (read_req.beatcount <= (((read_req.size + 8) / 8) + 1)) ? 1 : 0;
            #ifdef VERBOSE
            fprintf(stderr, "MEM: GET address %08lx beat %d = %016lx... id %d\n",
                    read_req.addr, read_req.beatcount, rdata, rid);
            #endif
            read_req.beatcount--;
            read_req.addr += 8;
            if (read_req.firstbeat) {
                read_req.beatcount++;
                read_req.firstbeat = 0;
            }
        }
    }

    // Handle write request
    if ((write_req.beatcount == 0) && (awvalid)) {
        write_req.addr = awaddr;
        write_req.id = awid;
        write_req.size = awsize;
        write_req.len = awlen;
        write_req.burst = awburst;
        if (awburst != BURST_INCR) {
            fprintf(stderr, "Unsupported burst type %d", awburst);
        }
        write_req.beatcount = get_beats(awsize, awlen);
        write_req.firstbeat = 1;
        write_req.bubble = 0;
        wready = 1;
        awready = 0;
    }
    else if (write_req.beatcount != 0) {
        // Processing write request
        wready = 1;
        awready = 0;
        if (write_req.bubble) {
            // Try to respond on B channel but wasn't accepted
            wready = 0;
            if (bready) {
                // Accepted
                awready = 1;
                write_req.beatcount = 0;
                #ifdef VERBOSE
                fprintf(stderr, "Accepted\n");
                #endif
            }
            else {
                bvalid = 1;
                awready = 0;
            }
        }
        else if (wvalid) {
            #ifdef VERBOSE
            fprintf(stderr, "MEM: PUT address %08lx beat %d = %016lx mask %02x...\n",
                    write_req.addr, write_req.beatcount, wdata, wstrb);
            #endif
            memmap_write(write_req.addr, wdata, wstrb);
            write_req.beatcount--;
            write_req.addr += 8;
            if (write_req.beatcount == 0) {
                // Finished burst
                bvalid = 1;
                bresp = RESP_OKAY;
                bid = write_req.id;
                if (!bready) {
                    write_req.bubble = 1;
                    write_req.beatcount = 1;
                    wready = 0;
                }
                else {
                    #ifdef VERBOSE
                    fprintf(stderr, "Accepted\n");
                    #endif
                }
            }
        }
    }
}