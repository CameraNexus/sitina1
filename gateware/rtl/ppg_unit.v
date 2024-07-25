//
// ppg_unit.v: Simple Pulse Generator
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

module ppg_unit #(
    parameter WIDTH = 16
)(
    input  wire              clk,
    input  wire              rstn,
    input  wire              trig,
    input  wire [WIDTH-1:0]  t_lead,
    input  wire [WIDTH-1:0]  t_hold,
    output wire              q,
    output wire              qbar
);
    reg  q_r;
    reg [WIDTH-1:0] cnt_r;
    reg [1:0] state;
    assign q = q_r;
    assign qbar = ~q_r;

    always @(posedge clk or negedge rstn) begin
        if (~rstn) begin
            state <= 2'b00;
            cnt_r <= 0;
            q_r <= 0;
        end else begin
            case (state)
                2'b00: begin // Idle
                    if (trig) begin
                        if (t_lead == 0) begin
                            if (t_hold == 0) begin
                                state <= 2'b00;
                                q_r <= 0;
                            end else begin
                                state <= 2'b10;
                                cnt_r <= t_hold;
                                q_r <= 1;
                            end
                        end else begin
                            state <= 2'b01;
                            cnt_r <= t_lead;
                        end
                    end 
                end
                2'b01: begin    // Lead
                    if (cnt_r == 0) begin
                        if (t_hold == 0) begin
                            state <= 2'b00;
                            q_r <= 0;
                        end else begin
                            state <= 2'b10;
                            cnt_r <= t_hold;
                            q_r <= 1;
                        end
                    end else begin
                        cnt_r <= cnt_r - 1;
                    end
                end
                2'b10: begin    // Hold
                    if (cnt_r == 0) begin
                        state <= 2'b00;
                        q_r <= 0;
                    end else begin
                        cnt_r <= cnt_r - 1;
                    end
                end
            endcase
        end
    end

endmodule /* ppg_unit */

`default_nettype wire
