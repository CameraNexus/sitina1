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
    input  wire [WIDTH-1:0]  tdelay,
    input  wire [WIDTH-1:0]  tpulse,
    output reg               q,
    output reg               qbar
);
    wire qbar_pre;
    reg  q_pre;
    reg [WIDTH-1:0] counter;
    always @* begin
        q_pre = 0;
        if(counter>tdelay-1) q_pre = 1'b1;
        if(counter>tpulse-1) q_pre = 1'b0;
    end

   assign qbar_pre = ~q_pre;
   always @(posedge clk or negedge rstn) begin
      if(~rstn)          counter <= 0;
      else if(trig)      counter <= 1;
      else if(counter>0) counter <= counter + 1;
   end

endmodule /* ppg_unit */

`default_nettype wire
