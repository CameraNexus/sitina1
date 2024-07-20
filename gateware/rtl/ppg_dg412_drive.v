//
// ppg_dg412_drive.v: DG412 Deadtime Control
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

module ppg_dg412_drive #(
    parameter WIDTH = 3
)(
    input  wire                 clk_fast,   // Fast clock
    input  wire                 rstn,       // System reset
    input  wire                 arm,        // Arm signal
    input  wire                 cki,        // Signal In
    output wire                 ckop,       // Signal Out Positive
    output wire                 ckon,       // Signal Out Negative
    input  wire [WIDTH-1:0]     tdt,        // Deadtime (number of fast clock cycles)
    output wire                 armed       // Armed signal, OR this with other modules to generate the dcdc_en signal
);
    reg [1:0] state_r;
    reg [WIDTH-1:0] cnt_r;
    reg arm_r;

    assign ckop = (state_r == 2'b10) & arm_r;
    assign ckon = (state_r == 2'b00) & arm_r;
    assign armed = arm_r;

    always @(posedge clk_fast or negedge rstn) begin
        if (~rstn) begin
            state_r <= 2'b00;
            cnt_r <= 0;
            arm_r <= 0;
        end else begin
            arm_r <= arm;
            case (state_r)
                2'b00: begin // Idle (Low)
                    if (cki) begin
                        if (tdt == 0) state_r <= 2'b10;
                        else          state_r <= 2'b01;
                        cnt_r <= tdt;
                    end 
                end
                2'b01: begin // Posedge
                    if (cnt_r == 1) state_r <= 2'b10;
                    cnt_r <= cnt_r - 1;
                end
                2'b10: begin // High
                    if (~cki) begin
                        if (tdt == 0) state_r <= 2'b00;
                        else          state_r <= 2'b11;
                        cnt_r <= tdt;
                    end
                end
                2'b11: begin // Negedge
                    if (cnt_r == 1) state_r <= 2'b00;
                    cnt_r <= cnt_r - 1;
                end
            endcase
        end
    end

endmodule
`default_nettype wire
//
// ppg_dg412_drive.v: DG412 Deadtime Control
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

module ppg_dg412_drive #(
    parameter WIDTH = 3
)(
    input  wire                 clk_fast,   // Fast clock
    input  wire                 rstn,       // System reset
    input  wire                 arm,        // Arm signal
    input  wire                 cki,        // Signal In
    output wire                 ckop,       // Signal Out Positive
    output wire                 ckon,       // Signal Out Negative
    input  wire [WIDTH-1:0]     tdt,        // Deadtime (number of fast clock cycles)
    output wire                 armed       // Armed signal, OR this with other modules to generate the dcdc_en signal
);
    reg [1:0] state_r;
    reg [WIDTH-1:0] cnt_r;
    reg arm_r;

    assign ckop = (state_r == 2'b10) & arm_r;
    assign ckon = (state_r == 2'b00) & arm_r;
    assign armed = arm_r;

    always @(posedge clk_fast or negedge rstn) begin
        if (~rstn) begin
            state_r <= 2'b00;
            cnt_r <= 0;
            arm_r <= 0;
        end else begin
            arm_r <= arm;
            case (state_r)
                2'b00: begin // Idle (Low)
                    if (cki) begin
                        if (tdt == 0) state_r <= 2'b10;
                        else          state_r <= 2'b01;
                        cnt_r <= tdt;
                    end 
                end
                2'b01: begin // Posedge
                    if (cnt_r == 1) state_r <= 2'b10;
                    cnt_r <= cnt_r - 1;
                end
                2'b10: begin // High
                    if (~cki) begin
                        if (tdt == 0) state_r <= 2'b00;
                        else          state_r <= 2'b11;
                        cnt_r <= tdt;
                    end
                end
                2'b11: begin // Negedge
                    if (cnt_r == 1) state_r <= 2'b00;
                    cnt_r <= cnt_r - 1;
                end
            endcase
        end
    end

endmodule
`default_nettype wire