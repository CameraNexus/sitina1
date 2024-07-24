//
// ppg_v1r.v: Pulse Generator for CCD V1 control with individual timers 
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
//------------------------------------------------------------------------------
// V1 TIMING DIAGRAM
// 
//  VDDL(0V)-  ‾‾‾‾‾‾‾^\  .               .  /^‾‾‾‾‾‾‾
//             - - - -.-\-.- - - - - - - -.-/-.- - - -
//  VLO(-9V)-  _ _ _ _._ \v_______________v/ _._ _ _ _
//                    .   .               .   .
//  TRIG-  ____/\_____.___._______________.___.____________
//             |<---->|  Lead             .   .
//             .      .|-| Posedge VDDL (PEL) .
//             .      .   |<------------->| Hold VDDL (LH1)
//             .      .   .               .|-| Negedge VLO (NEL)
//             .      .   .               .   |<-- Low (LO)
//             |<---->| PDN Lead          .   .           = 20
//             .      |<--------------------->| PDN Hold  = 1816
//             |<-------->| PLO Lead      .   .           = 340
//             .      .   |<------------->| PLO Hold      = 1176
//             |<----->| SELL0 Lead       .   .           = 30
//             .      .|-| SELL0 Hold     .   .           = 300
//             |<------------------------->| SELL1 Lead   = 1526
//             .      .   .                |-| SELL1 Hold = 300
// PDN-   ‾‾‾‾‾‾‾‾‾‾‾‾\___._______________.___/‾‾‾‾‾‾‾‾‾‾‾‾
// PLO-   _____.______.___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___.____________
// SELL-  _____.______./‾\._______________./‾\.____________
//       
// To implement tV3RD, the alternative timing control data need to be deposited 
// before the trigger signal is asserted. 
//
//------------------------------------------------------------------------------
//
// Calculate the parameters for the CCD V1 control:
//
// fCLKFast = 336M~224M Hz 
// tCLKFast = 2.98~4.46 ns
//
// txxx: Pulse Width, txxx_yyy: Dead Time Control (Can be < 0)
// 
// pdn_lead   = tLead   
// pdn_hold   =         tLO_PE + tPE + tPE_LH + tLH1 + tLH1_NE + tNE + tNE_LO
// plo_lead   = tLead + tLO_PE + tPE + tPE_LH
// plo_hold   =                                 tLH1
// sell0_lead = tLead + tLO_PE
// sell0_hold =                  tPE
// sell1_lead = tLead + tLO_PE + tPE + tPE_LH + tLH1 + tLH1_NE
// sell1_hold =                                                  tNE
// Typ. val   = 20      20       300   20       836    20        300   20
//              59.5ns   59.5ns   893ns 59.5ns   2.49us 59.5ns    893ns 59.5ns
//                                |<---------- 3.5us ----------->|
// Typ. val   = 20      .        .     .        3020   .         .     .
// (V3RD)       59.5ns  .        .     .        10.0us .         .     .
//                                |<---------- 10.0us ---------->|
//              ^ To align with the V2 posedge (t3P) in V3RD mode
//
//------------------------------------------------------------------------------
`timescale 1ns / 1ps
`default_nettype none

module ppg_v1r #(
    parameter DW = 13,      // Data Width
    parameter CAPTURE = 0   // Capture the trigger signal?
)(
    input wire              clk_fast, // Fast clock, at least 336MHz (3x 112MHz)
    input wire              rstn,
    input wire              trig,

    input wire [DW-1:0]     t_pdn_lead,
    input wire [DW-1:0]     t_pdn_hold,
    input wire [DW-1:0]     t_plo_lead,
    input wire [DW-1:0]     t_plo_hold,
    input wire [DW-1:0]     t_sell0_lead,
    input wire [DW-1:0]     t_sell0_hold,
    input wire [DW-1:0]     t_sell1_lead,
    input wire [DW-1:0]     t_sell1_hold,

    output wire             q_sell,
    output wire             q_pdn,
    output wire             q_plo
);

    // Trig Capture
    reg trig_capture_r;
    if (CAPTURE) begin
        always @(posedge clk_fast or negedge rstn) begin
            if(~rstn) begin
                trig_capture_r <= 1'b0;
            end else begin
                if(~trig_capture_r&trig) begin
                    trig_capture_r <= 1'b1;
                end else begin
                    trig_capture_r <= 1'b0;
                end
            end
        end
    end else begin
        always @* begin
            trig_capture_r = trig;
        end
    end

    wire q_sell0;
    wire q_sell1;
    assign q_sell = q_sell1 | q_sell0;

    ppg_unit #(.WIDTH(DW)) u_pdn(
        .clk(clk_fast),.rstn,.trig(trig_capture_r),
        .t_lead(t_pdn_lead),.t_hold(t_pdn_hold),
        .q(),.qbar(q_pdn)
    );

    ppg_unit #(.WIDTH(DW)) u_plo(
        .clk(clk_fast),.rstn,.trig(trig_capture_r),
        .t_lead(t_plo_lead),.t_hold(t_plo_hold),
        .q(q_plo),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_sell0(
        .clk(clk_fast),.rstn,.trig(trig_capture_r),
        .t_lead(t_sell0_lead),.t_hold(t_sell0_hold),
        .q(q_sell0),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_sell1(
        .clk(clk_fast),.rstn,.trig(trig_capture_r),
        .t_lead(t_sell1_lead),.t_hold(t_sell1_hold),
        .q(q_sell1),.qbar()
    );

endmodule /* ppg_v1r */