`timescale 1ns / 1ps
`default_nettype none
//
// ccdclk.v: 2X and 4X CCD clock generator
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
module ccdclk (
    input wire clk,
    input wire rst,
    output wire clk_2x,
    output wire rst_2x,
    output wire clk_4x,
    output wire rst_4x,
    output wire locked
);

    wire clkfbout;
    wire clkout0;
    wire clkout1;
    wire clkfbout_buf;
    wire mmcm_locked;
    PLLE2_ADV #(
        .BANDWIDTH            ("OPTIMIZED"),
        .CLKFBOUT_MULT        (32.000), // 864MHz
        .CLKFBOUT_PHASE       (0.000),
        .CLKIN1_PERIOD        (37.04), // 27MHz
        .CLKOUT0_DIVIDE       (8.000),  // 108MHz
        .CLKOUT0_PHASE        (0.000),
        .CLKOUT0_DUTY_CYCLE   (0.500),
        .CLKOUT1_DIVIDE       (16.000),  // 54MHz
        .CLKOUT1_PHASE        (90.000),
        .CLKOUT1_DUTY_CYCLE   (0.500),
        .COMPENSATION         ("ZHOLD"),
        .DIVCLK_DIVIDE        (1),
        .REF_JITTER1          (0.05),
        .STARTUP_WAIT         ("FALSE")
    ) mmcm_adv_inst (
        // Output clocks
        .CLKFBOUT            (clkfbout),
        .CLKOUT0             (clkout0),
        .CLKOUT1             (clkout1),
        .CLKOUT2             (),
        .CLKOUT3             (),
        .CLKOUT4             (),
        .CLKOUT5             (),
        // Input clock control
        .CLKFBIN             (clkfbout_buf),
        .CLKIN1              (clk),
        .CLKIN2              (1'b0),
        // Tied to always select the primary input clock
        .CLKINSEL            (1'b1),
        // Ports for dynamic reconfiguration
        .DADDR               (7'h0),
        .DCLK                (1'b0),
        .DEN                 (1'b0),
        .DI                  (16'h0),
        .DO                  (),
        .DRDY                (),
        .DWE                 (1'b0),
        // Other control and status signals
        .LOCKED              (mmcm_locked),
        .PWRDWN              (1'b0),
        .RST                 (rst)
    );

    assign locked = mmcm_locked;

    BUFG clkfb_buf (
        .I(clkfbout),
        .O(clkfbout_buf)
    );

    BUFG clkout0_buf (
        .I(clkout0),
        .O(clk_4x)
    );

    BUFG clkout1_buf (
        .I(clkout1),
        .O(clk_2x)
    );

    xpm_cdc_sync_rst rst_2x_sync(
        .dest_clk(clk_2x),
        .src_rst(!mmcm_locked),
        .dest_rst(rst_2x)
    );

    xpm_cdc_sync_rst rst_4x_sync(
        .dest_clk(clk_4x),
        .src_rst(!mmcm_locked),
        .dest_rst(rst_4x)
    );

endmodule

`default_nettype wire
