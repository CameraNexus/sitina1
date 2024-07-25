//
// ppg_v2r.v: Pulse Generator for CCD V2 control with individual timers 
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
// V2 TIMING DIAGRAM
//
//  VDDH(8V)-  ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾/^‾‾‾‾‾^\‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾ ‾
//                   .   .     . / .     . \ .     .   .
//  VDDL(0V)-  - - - - -.x-----x'- - - - - -'x-----x.- - - - - -
//                   . / .     .   .     .   .     . \ .
//  VLO(-9V)-  ______v/_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\v________
//                   .   .     .   .     .   .     .   .
//  TRIGL  ____/\____.___._____.___._____.___._____.___.________
//  TRIGH  ____._____.___.__/\_.___._____.___._____.___.________
//             |<--->|  Lead   .   .     .   .     .   .
//             .      |-|  Posedge VDDL (PEL)      .   .
//             .     .   |<--->|  Hold VDDL 1 (LH1).   .
//             .     .   .  .   |-|  Posedge VDDH (PEH)
//             .     .   .  .  .   |<--->|  Hold VDDH (HH)
//             .     .   .  .  .   .      |-|  Negedge VDDH (NEH)
//             .     .   .  .  .   .     .   |<--->|  Hold VDDL 2 (LH2)
//             .     .   .  .  .   .     .   .      |-|  Negedge VLO (NEL)
//             .     .   .  .  .   .     .   .     .   |<---- Low (LO)
//             |<--->| PDN Lead.   .     .   .     .   .
//             .     |<------------------------------->| PDN Hold  
//             |<---->| SELL0 Lead .     .   .     .   .
//             .     .|-| SELL0 Hold     .   .     .   .
//             .     .   .  |<--------------------->| SELL1 Lead
//             .     .   .  .  .   .     .   .     .|-| SELL1 Hold
//             |<------->| PLO0 Lead     .   .     .   .
//             .     .   |<--->| PLO0 Hold   .     .   .
//             |<--------------------------->| PLO1 Lead (non V3RD)
//             .     .   .  |<-------------->| PLO1 Lead (V3RD)
//             .     .   .  .  .   .     .   |<--->| PLO1 Hold
//             .     .   .  |<->| SELH0 Lead .     .   .
//             .     .   .  .  .|-| SELH0 Hold     .   .
//             .     .   .  |<----------->| SELH1 Lead .
//             .     .   .  .  .   .     .|-| SELH1 Hold
//             .     .   .  |<---->| PHI Lead.     .   .
//             .     .   .  .  .   |<--->| PHI Hold.   .
// PDN-   ‾‾‾‾‾‾‾‾‾‾‾\___.__.__.___._____.___._____.___/‾‾‾‾‾‾‾‾‾‾‾
// SELL-  ___________./‾\.__.__.___._____.___._____./‾\.___________
// PLO-   ___________.___/‾‾‾‾‾\___._____.___/‾‾‾‾‾\___.___________
// SELH-  ___________.___.__.__./‾\._____./‾\._____.___.___________
// PHI-   ___________.___.__.__.___/‾‾‾‾‾\___._____.___.___________
//                   .   .     .   .     .   .     .   .           
// SELHGUARD ________.___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___.___________ (Internal) 
// SELHL  ‾‾‾‾‾‾‾‾‾‾‾‾‾‾\___.__.__.___._____.___.___/‾‾‾‾‾‾‾‾‾‾‾‾‾‾ 
// SELHM  ___________.___/‾‾‾‾‾\___/‾‾‾‾‾\___/‾‾‾‾‾\___.___________
// SELHH  ___________.___.__.__./‾\._____./‾\._____.___.___________
// 
//------------------------------------------------------------------------------
//
// Calculate the parameters for the CCD V2 control:
//
// fCLKFast = 336M~224M Hz
// tCLKFast = 2.98~4.46 ns
//
// txxx: Pulse Width, txxx_yyy: Dead Time Control (Can be < 0)
//
//               tLeadl      tLH1        
//               |  tLO_PEL  |  tLH2_NEL
//               |  |  tPEL  |  |  tNEL
//               |  |  |  tPEL_LH1 |  tNEL_LO
//               |  |  |  |  |  |  |  | 
// pdn        =  L  H  H  H  H  H  H  H
// sell0      =  L  L  H  .  .  .  .  .
// plo0       =  L  L  L  L  H  .  .  .
// sell1      =  L  L  L  L  L  L  H  .
//               |  |  |  |  |  |  |  | 
// Typ. val      |  20 |  20 |  20 |  20
//               0     300   876   300
//               
//                            v
//                            v
//               tLeadl      tLH1        tHH         tLH2
//               |  tLO_PEL  |  tLH1_PEH |  tHH_NEH  |  tLH2_NEL
//               |  |  tPEL  |  |  tPEH  |  |  tNEH  |  |  tNEL
//               |  |  |  tPEL_LH1 |  tPEH_HH  |  tNEH_LH2 |  tNEL_LO
//               |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
// pdn        =  L  H  H  H  x  x  x  x  x  x  x  x  x  x  x  x
// sell0      =  L  L  H  .  .  .  .  .  .  .  .  .  .  .  .  .
// plo0       =  L  L  L  L  H  .  .  .  .  .  .  .  .  .  .  .
// selh0      =  .  .  .  .  .  L  H  .  .  .  .  .  .  .  .  .
// phi        =  .  .  .  .  .  L  L  L  H  .  .  .  .  .  .  .
// selh1      =  .  .  .  .  .  L  L  L  L  L  H  .  .  .  .  .
// plo1       =  .  .  .  .  .  L  L  L  L  L  L  L  H  .  .  .
// sell1      =  .  .  .  .  .  .  .  .  .  .  L  L  L  L  H  .
// pdn1       =  .  .  .  .  .  .  .  .  .  .  H  H  H  H  H  H
//               |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |
// Typ. val      |  20 |  19 |  20 |  20 |  20 |  20 |  20 |  20 
// (V3RD)        0     300   >=1   300   3020  300   6380  300
//                             ^
//                             ^ TRIG_H
//------------------------------------------------------------------------------


`timescale 1ns / 1ps
`default_nettype none

module ppg_v2r #(
    parameter DW = 13,      // Data Width
    parameter CAPTURE = 0   // Capture the trigger signal?
)(
    input wire              clk_fast, // Fast clock, at least 336MHz (3x 112MHz)
    input wire              rstn,
    input wire              trig_l,
    input wire              trig_h,
    input wire              v3mode,

    input wire [DW-1:0]     t_pdn0_lead,
    input wire [DW-1:0]     t_pdn0_hold,
    input wire [DW-1:0]     t_sell0_lead,
    input wire [DW-1:0]     t_sell0_hold,
    input wire [DW-1:0]     t_plo0_lead,
    input wire [DW-1:0]     t_plo0_hold,
    input wire [DW-1:0]     t_selh0_lead,
    input wire [DW-1:0]     t_selh0_hold,
    input wire [DW-1:0]     t_phi_lead,
    input wire [DW-1:0]     t_phi_hold,
    input wire [DW-1:0]     t_selh1_lead,
    input wire [DW-1:0]     t_selh1_hold,
    input wire [DW-1:0]     t_plo1_lead,
    input wire [DW-1:0]     t_plo1_hold,
    input wire [DW-1:0]     t_pdn1_lead,
    input wire [DW-1:0]     t_pdn1_hold,
    input wire [DW-1:0]     t_sell1_lead,
    input wire [DW-1:0]     t_sell1_hold,

    output wire             q_sell,
    output wire             q_pdn,
    output wire             q_plo,
    output wire             q_selh,
    output wire             q_phi,
    output wire             q_selh_guard
);

    // Trig Capture
    reg trig_internal_plo1_r;
    wire trig_l_capture;
    wire trig_h_capture;
    wire trig_plo1_capture;
    reg trig_l_dly_r;
    reg trig_h_dly_r;
    reg trig_plo1_dly_r;
    always @(posedge clk_fast or negedge rstn) begin
        if(~rstn) begin
            trig_l_dly_r <= 0;
            trig_h_dly_r <= 0;
            trig_plo1_dly_r <= 0;
        end else begin
            trig_l_dly_r <= trig_l;
            trig_h_dly_r <= trig_h;
            trig_plo1_dly_r <= trig_internal_plo1_r;
        end
    end
    assign trig_plo1_capture = ~trig_plo1_dly_r & trig_internal_plo1_r;
    if (CAPTURE) begin
        assign trig_l_capture    = ~trig_l_dly_r & trig_l;
        assign trig_h_capture    = ~trig_h_dly_r & trig_h;
    end else begin
        assign trig_l_capture    = trig_l;
        assign trig_h_capture    = trig_h;
    end
    
    wire q_pdn0, q_pdn1;
    wire q_sell0, q_sell1;
    wire q_selh0, q_selh1;
    wire q_plo0, q_plo1;

    
    reg lh1_holder_r;
    assign q_sell = q_sell1 | q_sell0;
    assign q_plo  = q_plo1  | q_plo0 | lh1_holder_r;
    assign q_selh = q_selh1 | q_selh0;
    // assign q_selh_guard = lh1_holder_r;
    assign q_selh_guard = q_plo;
    assign q_pdn  = ~(q_pdn0 | q_pdn1 | lh1_holder_r);

    always @(posedge clk_fast or negedge rstn) begin
        if (~rstn) begin
            lh1_holder_r         <= 1'b0;
            trig_internal_plo1_r <= 1'b0;
        end else begin
            if(q_plo1)              lh1_holder_r <= 1'b0;
            else if(q_plo0&&v3mode) lh1_holder_r <= 1'b1;
            if(q_pdn)               trig_internal_plo1_r <= 1'b0;
            else if(q_selh1)         trig_internal_plo1_r <= 1'b1;
        end
    end

    ppg_unit #(.WIDTH(DW)) u_pdn(
        .clk(clk_fast),.rstn,.trig(trig_l_capture),
        .t_lead(t_pdn0_lead),.t_hold(t_pdn0_hold),
        .q(q_pdn0),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_sell0(
        .clk(clk_fast),.rstn,.trig(trig_l_capture),
        .t_lead(t_sell0_lead),.t_hold(t_sell0_hold),
        .q(q_sell0),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_plo0(
        .clk(clk_fast),.rstn,.trig(trig_l_capture),
        .t_lead(t_plo0_lead),.t_hold(t_plo0_hold),
        .q(q_plo0),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_selh0(
        .clk(clk_fast),.rstn,.trig(trig_h_capture),
        .t_lead(t_selh0_lead),.t_hold(t_selh0_hold),
        .q(q_selh0),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_phi(
        .clk(clk_fast),.rstn,.trig(trig_h_capture),
        .t_lead(t_phi_lead),.t_hold(t_phi_hold),
        .q(q_phi),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_selh1(
        .clk(clk_fast),.rstn,.trig(trig_h_capture),
        .t_lead(t_selh1_lead),.t_hold(t_selh1_hold),
        .q(q_selh1),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_plo1(
        .clk(clk_fast),.rstn,.trig(trig_h_capture),
        .t_lead(t_plo1_lead),.t_hold(t_plo1_hold),
        .q(q_plo1),.qbar()
    );

    ppg_unit #(.WIDTH(DW)) u_sell1(
        .clk(clk_fast),.rstn,.trig(v3mode?trig_plo1_capture:trig_l_capture),
        .t_lead(t_sell1_lead),.t_hold(t_sell1_hold),
        .q(q_sell1),.qbar()
    );
    ppg_unit #(.WIDTH(DW)) u_pdn1(
        .clk(clk_fast),.rstn,.trig(trig_plo1_capture),
        .t_lead(t_pdn1_lead),.t_hold(t_pdn1_hold),
        .q(q_pdn1),.qbar()
    );

endmodule /* ppg_v1r */