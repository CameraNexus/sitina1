`timescale 1ns / 1ps
`default_nettype none
`include "mu_defines.vh"
//
// system.v: Portable system top level
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
module system #(
    parameter AXI_AW = 32,
    parameter AXI_DW = 64,
    parameter AXI_IDW = 2
) (
    input  wire         clk,        // Clock
    input  wire         clk_pix,    // Pixel clock
    input  wire         rst,
    input  wire         rst_pix,
    // DVP from AD9990
    input  wire [13:0]  dvp_d,
    input  wire         dvp_hsync,
    input  wire         dvp_vsync,
    input  wire         dvp_pclk,
    // AD9990 control
    output wire         afe_rst,
    output wire         afe_sync,
    output wire         afe_sdata,
    output wire         afe_sl,
    output wire         afe_sck,
    // MIPI DSI for LCD
    output wire         dsi_lp_cp,
    output wire         dsi_lp_cn,
    output wire [1:0]   dsi_lp_dp,
    output wire [1:0]   dsi_lp_dn,
    output wire         dsi_lp_sel,
    // TCON for CCD
    output wire         tcon_v1,
    output wire         tcon_v2,
    output wire         tcon_v23,
    output wire         tcon_fdg,
    output wire         tcon_strobe,
    output wire         tcon_h1,
    output wire         tcon_h2,
    output wire         tcon_rg,
    // Power control
    output wire         vab_pwm,
    // APB device port for register access
    `APB_SLAVE_IF,
    // AXI host port for memory access
    `AXI_MASTER_IF(AXI_AW, AXI_DW, AXI_IDW)
);

    // Number of APB devices
    localparam APB_N = 1;

    // Internal APB for register access
    wire regbus_pwrite;
    wire [31:0] regbus_pwdata;
    wire [31:0] regbus_paddr;
    wire regbus_penable;
    wire [APB_N-1:0] regbus_psel_vec;
    wire [APB_N-1:0] regbus_pready_vec;
    wire [APB_N*32-1:0] regbus_prdata_vec;

	apbbus #(
        .N(APB_N)
    ) apbbus (
        // Interface to APB master
        .up_pwrite(s_apb_pwrite),
        .up_pwdata(s_apb_pwdata),
        .up_paddr(s_apb_paddr),
        .up_penable(s_apb_penable),
        .up_psel(s_apb_psel),
        .up_pready(s_apb_pready),
        .up_prdata(s_apb_prdata),
        // Interface to APB slaves
        .down_pwrite(regbus_pwrite),
        .down_pwdata(regbus_pwdata),
        .down_paddr(regbus_paddr),
        .down_penable(regbus_penable),
        .down_psel_vec(regbus_psel_vec),
        .down_pready_vec(regbus_pready_vec),
        .down_prdata_vec(regbus_prdata_vec)
    );

    wire [31:0] gpio_out;
    wire [31:0] gpio_in;
    wire [31:0] gpio_oe;
    mu_gpio mu_gpio (
        .clk(clk),
        .rst(rst),
        // APB device port for register access
        `APB_SLAVE_CONN(regbus_, 0),
        // GPIO
        .gpio_out(gpio_out),
        .gpio_in(gpio_in),
        .gpio_oe(gpio_oe)
    );

    assign afe_rst = gpio_out[0];
    assign afe_sync = gpio_out[1];
    assign afe_sdata = gpio_out[2];
    assign afe_sl = gpio_out[3];
    assign afe_sck = gpio_out[4];

    assign dsi_lp_sel = gpio_out[5];
    assign dsi_lp_cp = gpio_out[6];
    assign dsi_lp_cn = gpio_out[7];
    assign dsi_lp_dp = gpio_out[9:8];
    assign dsi_lp_dn = gpio_out[11:10];

endmodule

`default_nettype wire
