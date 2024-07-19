//
// ppg_v1.v: Pulse Generator for CCD V1 control 
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

// Terminology:
//  VDDL(0V)-           /‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\
//                     /.                           .\
//  VLO(-9V)-  _______/ .                           . \_______
//                    . .                           . .
//  TRIG-  ____/\_____._.___________________________._.________
//             |<---->|  Lead                       . .
//                    || td_lead_pel                . .
//                    |-|  Posedge VDDL (PEL)      . .
//                    . || td_pel_lh               . .
//                    . |<------------------------>|  Hold VDDL (LH1)
//                    . .                          || td_lh_nel
//                    . .                          |-|  Negedge VLO (NEL)
//                    . .                          . || td_nel
//                    . .                          . |<---- Low (LO)
// PDN-   ‾‾‾‾‾‾‾‾‾‾‾‾\_.__________________________._/‾‾‾‾‾‾‾‾‾‾‾‾
// PLO-   ____________._/‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾\_.____________
// SELL-  ____________/‾\__________________________/‾\____________
//           

`timescale 1ns / 1ps
`default_nettype none

module ppg_v1 #(
    parameter WIDTH = 16
)(
    input wire                     clk,
    input wire                     rstn,
    input wire                     trig,
    input wire        [WIDTH-1:0]  t_lead,      // Lead time
    input wire signed [WIDTH-1:0]  td_lead_pel, // Lead-to-Posedge Dead Time 
    input wire        [WIDTH-1:0]  t_pel,       // Posedge Edge VDDL
    input wire signed [WIDTH-1:0]  td_pel_lh,   // Posedge-to-Hold Dead Time
    input wire        [WIDTH-1:0]  t_lh,        // VDDL hold time
    input wire signed [WIDTH-1:0]  td_lh_nel,   // Hold-to-Negedge Dead Time
    input wire        [WIDTH-1:0]  t_nel,       // Nededge VDDH
    input wire signed [WIDTH-1:0]  td_nel,      // Negedge-to-VLO Dead Time
    output wire                    invalid,     // Some configuration is invalid
    output reg                     q_sell,      // VDDL LC switch
    output reg                     q_pdn,       // Pull Down
    output reg                     q_plo        // Pull to VDDL
);

    // Calculate all the timings
    // The module is designed to be dynamically reconfigurable, so the timings 
    // are calculated in the module

    wire signed [WIDTH  :0] xt_sell1_signex;
    wire        [WIDTH-1:0] xt_sell1 = xt_sell1_signex[WIDTH-1:0];
    wire signed [WIDTH  :0] xt_plo_signex;
    wire        [WIDTH-1:0] xt_plo   = xt_plo_signex[WIDTH-1:0];
    wire signed [WIDTH  :0] xt_sell2_signex;
    wire        [WIDTH-1:0] xt_sell2 = xt_sell2_signex[WIDTH-1:0];
    wire signed [WIDTH  :0] xt_pdn_signex;
    wire        [WIDTH-1:0] xt_pdn   = xt_pdn_signex[WIDTH-1:0];

    assign invalid = xt_sell1[WIDTH]|xt_plo[WIDTH]|xt_sell2[WIDTH]|xt_pdn[WIDTH];

    assign xt_sell1_signex  = {t_lead[WIDTH-1],  t_lead  }+{td_lead_pel[WIDTH-1],td_lead_pel};
    assign xt_plo_signex    = {xt_sell1[WIDTH-1],xt_sell1}+{td_pel_lh[WIDTH-1],  td_pel_lh  };
    assign xt_sell2_signex  = {xt_plo[WIDTH-1],  xt_plo  }+{td_lh_nel[WIDTH-1],  td_lh_nel  };

    wire q_sell1;
    wire q_sell2;
    wire q_pdn1;
    wire q_plo1;

    ppg_unit #(.WIDTH(WIDTH)) u_pg_sell1(
        .clk(clk),.rstn(rstn),.trig(trig),
        .tdelay(xt_sell1),.tpulse(t_pel),
        .q(q_sell1),.qbar()
    );

    ppg_unit #(.WIDTH(WIDTH)) u_pg_sell2(
        .clk(clk),.rstn(rstn),.trig(trig),
        .tdelay(xt_sell2),.tpulse(t_nel),
        .q(q_sell2),.qbar()
    );

    ppg_unit #(.WIDTH(WIDTH)) u_pg_pdn1(
        .clk(clk),.rstn(rstn),.trig(trig),
        .tdelay(xt_pdn),.tpulse(t_pel),
        .q(),.qbar(q_pdn1)
    );

    ppg_unit #(.WIDTH(WIDTH)) u_pg_plo1(
        .clk(clk),.rstn(rstn),.trig(trig),
        .tdelay(xt_plo),.tpulse(t_lh),
        .q(q_plo1),.qbar()
    );

    always @(posedge clk or negedge rstn) begin
        if(~rstn) begin
            q_sell <= 1'b0;
            q_pdn  <= 1'b0;
            q_plo  <= 1'b0;
        end else begin
            q_sell <= q_sell1|q_sell2;
            q_pdn  <= ~q_pdn1;
            q_plo  <= q_plo1;
        end
    end
endmodule /* ppg_v1 */

`default_nettype wire
