`timescale 1ns / 1ps
`default_nettype none
//
// apbbus.v: APB bus interconnect
//
// Copyright 2024 Wenting Zhang <zephray@outlook.com>
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

module apbbus #(
    parameter N = 4     // Number of slaves
) (
    // Interface to APB master
    input  wire             up_pwrite,
    input  wire [31:0]      up_pwdata,
    input  wire [31:0]      up_paddr,
    input  wire             up_penable,
    input  wire             up_psel,
    output reg              up_pready,
    output reg  [31:0]      up_prdata,
    // Interface to APB slaves
    output wire             down_pwrite,
    output wire [31:0]      down_pwdata,
    output wire [31:0]      down_paddr,
    output wire             down_penable,
    output reg  [N-1:0]     down_psel_vec,
    input  wire [N-1:0]     down_pready_vec,
    input  wire [N*32-1:0]  down_prdata_vec
);

    assign down_pwrite = up_pwrite;
    assign down_pwdata = up_pwdata;
    assign down_paddr = up_paddr;
    assign down_penable = up_penable;

    // Each down stream device gets 64KB space (16-bit)
    // Decoding starts with bit 16
    wire [1:0] dec_addr = up_paddr[17:16];

    integer i;
    always @(*) begin
        up_pready = 1'b1;
        up_prdata = 32'hdeadbeef;
        down_psel_vec = 'd0;
        for (i = 0; i < N; i = i + 1) begin
            if ((dec_addr == i[1:0]) && up_psel) begin
                down_psel_vec[i] = 1'b1;
                up_pready = down_pready_vec[i];
                up_prdata = down_prdata_vec[i*32+:32];
            end
        end
    end

endmodule

`default_nettype wire
