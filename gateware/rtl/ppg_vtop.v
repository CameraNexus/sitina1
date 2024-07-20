//
// ppg_vtop.v: Pulse Generator for CCD V Drivers
//
// Copyright 2024 Anhang Li <thelithcore@gmail.com>
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

`timescale 1ns / 1ps
`default_nettype none
`include "mu_defines.vh"

module ppg_vtop(
    input wire clk_fast,    // Fast clock, at least 2x clk_ccd
    input wire rst_ccd,

    // APB device port for register access
    input  wire         s_apb_pwrite,
    input  wire [31:0]  s_apb_pwdata,
    input  wire [31:0]  s_apb_paddr,
    input  wire         s_apb_penable,
    input  wire         s_apb_psel,
    output reg          s_apb_pready,
    output reg [31:0]   s_apb_prdata,

    // From CCD Timing Gen
    input wire tcon_v1_trig,
    input wire tcon_v2_trig,
    input wire tcon_v23_trig,

    // Status Report    
    input wire arm_ccd,     // Assert this only after all the inits are done
    output wire ppg_armed,  // 

    // To V1 DG412 Gate Drivers
    output wire drv_v1_sell_hi,
    output wire drv_v1_sell_lo,
    output wire drv_v1_pdn_hi, 
    output wire drv_v1_pdn_lo,
    output wire drv_v1_plo_hi,
    output wire drv_v1_plo_lo,
    // To V2 DG412 Gate Drivers
    output wire drv_v2_sell_hi,
    output wire drv_v2_sell_lo,
    output wire drv_v2_pdn_hi,
    output wire drv_v2_pdn_lo,
    output wire drv_v2_plo_hi,
    output wire drv_v2_plo_lo,
    output wire drv_v2_selh_hi,
    output wire drv_v2_selh_mid,
    output wire drv_v2_selh_lo,
    output wire drv_v2_phi_hi,
    output wire drv_v2_phi_lo
);

    wire drv_v1_sell;
    wire drv_v1_pdn;
    wire drv_v1_plo;
    wire drv_v2_sell;
    wire drv_v2_pdn;
    wire drv_v2_plo;
    wire drv_v2_phi;
    // SELH of V2 is treated specially to avoid damaging the HV pass gate
    // wire drv_v2_selh_hi;
    // wire drv_v2_selh_mid;
    // ßwire drv_v2_selh_lo;

    localparam VCON_DW = 16;

    reg        [VCON_DW-1:0]  v1t_lead;       // Lead time
    reg signed [VCON_DW-1:0]  v1td_lead_pel;  // Lead-to-Posedge Dead Time 
    reg        [VCON_DW-1:0]  v1t_pel;        // Posedge Edge VDDL
    reg signed [VCON_DW-1:0]  v1td_pel_lh;    // Posedge-to-Hold Dead Time
    reg        [VCON_DW-1:0]  v1t_lh;         // VDDL hold time
    reg signed [VCON_DW-1:0]  v1td_lh_nel;    // Hold-to-Negedge Dead Time
    reg        [VCON_DW-1:0]  v1t_nel;        // Nededge VDDH
    reg signed [VCON_DW-1:0]  v1td_nel;       // Negedge-to-VLO Dead Time

    ppg_v1 #(WIDTH=VCON_DW)
    u_v1(
        .clk(clk_fast),
        .rstn(~rst_ccd),
        .trig(tcon_v1),
        .t_lead(v1t_lead),
        .td_lead_pel(v1td_lead_pel),
        .t_pel(v1t_pel),
        .td_pel_lh(v1td_pel_lh),
        .t_lh(v1t_lh),
        .td_lh_nel(v1td_lh_nel),
        .t_nel(v1t_nel),
        .td_nel(v1td_nel),
        .invalid(),
        .q_sell(drv_v1_sell),
        .q_pdn(drv_v1_pdn),
        .q_plo(drv_v1_plo)
    );

    wire [2:0] v1_armed;

    ppg_dg412_drive u_dg412_sell(
        .clk_fast(clk_fast),
        .rstn(~rst_ccd),
        .arm(arm_ccd),
        .cki(drv_v1_sell),
        .ckop(drv_v1_sell_hi),
        .ckon(drv_v1_sell_lo),
        .tdt(1),
        .armed(v1_armed[0])
    );

    ppg_dg412_drive u_dg412_pdn(
        .clk_fast(clk_fast),
        .rstn(~rst_ccd),
        .arm(arm_ccd),
        .cki(drv_v1_pdn),
        .ckop(drv_v1_pdn_hi),
        .ckon(drv_v1_pdn_lo),
        .tdt(1),
        .armed(v1_armed[1])
    );

    ppg_dg412_drive u_dg412_plo(
        .clk_fast(clk_fast),
        .rstn(~rst_ccd),
        .arm(arm_ccd),
        .cki(drv_v1_plo),
        .ckop(drv_v1_plo_hi),
        .ckon(drv_v1_plo_lo),
        .tdt(1),
        .armed(v1_armed[2])
    );

    // APB Slave Interface for Register Access
    always @(posedge clk_fast or negedge rst_ccd) begin
        if (~rst_ccd) begin
            s_apb_prdata <= 32'h0;
            s_apb_pready <= 1'b0;
        end else begin
            if (s_apb_psel && s_apb_penable) begin
                if (s_apb_pwrite) begin
                    case (s_apb_paddr)
                        32'h0:   {v1t_lead, v1td_lead_pel}  <= s_apb_pwdata;
                        32'h4:   {v1t_pel, v1td_pel_lh}     <= s_apb_pwdata;
                        32'h8:   {v1t_lh, v1td_lh_nel}      <= s_apb_pwdata;
                        32'hC:   {v1t_nel, v1td_nel}        <= s_apb_pwdata; 
                    endcase
                end else begin
                    case (s_apb_paddr)
                        32'h0:   s_apb_prdata <= {v1t_lead, v1td_lead_pel};
                        32'h4:   s_apb_prdata <= {v1t_pel, v1td_pel_lh};
                        32'h8:   s_apb_prdata <= {v1t_lh, v1td_lh_nel};
                        32'hC:   s_apb_prdata <= {v1t_nel, v1td_nel};
                        default: s_apb_prdata <= 32'h0; // Invalid register address
                    endcase
                end
                s_apb_pready <= 1'b1;
            end else begin
                s_apb_pready <= 1'b0;
            end
        end
    end



endmodule /* ppg_vtop */
`default_nettype wire