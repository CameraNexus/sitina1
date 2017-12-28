`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// ProjectUshi Verilog Code
// Copyright 2018 Wenting Zhang 
//
// Create Date:    13:39:17 12/14/2017 
// Module Name:    tim_vfsm
// Description:    Vertical Timing FSM
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//   Register Table:
//   00: Reserved
//   01: Shutter Time [23:16]
//   02: Shutter Time [15:8]
//   03: Shutter Time [7:0]
//////////////////////////////////////////////////////////////////////////////////
module tim_vfsm(
    input clk, //PIXCLK 30MHz, 33.3ns
    input rst,
    input trigger, //Trigger Input, can be set to always high
    input [1:0] a, //Register address
    input [7:0] d, //Register data
	 input we,      //Register write
    output reg [1:0] v1 = 2'b00, //2'b00 Low 2'b01 Mid 2'b10 High
    output reg v2 = 1,
    output reg shut = 0,
    output reg vact = 0,    //Line Active
	 output firstline,
	 output reg done = 0
    );

localparam TVCCD = 180;   // 6us / 33.3ns = 180
localparam THDS  = 180;   // 6us
localparam TS    = 120;   // 4us
localparam T3P   = 18000; // 600us
localparam TV3RD = 360;   // 12us
localparam T3D   = 600;   // 20us
localparam TD1L  = 1800;  // 60us
localparam THD   = 180;   // 6us
localparam TACT  = 2492;  // Line pixel count
localparam TL    = THD + TACT; //THD + Line Pixel Count
localparam THL   = 2;     // 50ns
localparam LINES = 3324;

//USE FIXED TIME FOR EXPERIMENT
//localparam TINT  = 100;   // 1/80s

//wire clk_shut; //Clock for Shutter speed counter, need to be 8kHz
//clk_divider #(1875) clk_divider_shut(clk, rst, clk_shut); 

reg [3:0] state = 0;
//do not need next_state
reg [15:0] main_counter = 0; //33.3ns * 65536 = 2182us max time
reg [19:0] shut_value = 0; //125us * 1048576 = 131.072s max shutter
reg shut_load = 0;
wire shut_done;
reg [11:0] line_counter = 0;

assign firstline = (line_counter == 0) ? 1'b1 : 1'b0;

timer #(20) timer_shut (clk, shut_load, shut_value, , shut_done);

always @(posedge clk)
begin
	if (rst == 1) begin
		state <= 4'd0;
		line_counter <= 0;
		v1 <= 2'b00;
		v2 <= 1'b1;
		shut <= 1'b0;
		vact <= 1'b0;
		shut_load <= 1'b1;
		shut_value <= 2'd2;
		done <= 1'b0;
	end
	else begin
		case(state)
			4'd0: 
			begin
				v1 <= 2'b00;
				v2 <= 1'b1;
				vact <= 1'b0;
				line_counter <= 1'b0;
				shut_load <= 1'b1;
				if (trigger == 1'b1) begin
					state <= 4'd1;
					main_counter <= 1'b0;
				end
			end
			4'd1:
			begin
				v1 <= 2'b01;
				v2 <= 1'b0;
				done <= 1'b0;
				if (main_counter == TVCCD) begin
					state <= 4'd2;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd2:
			begin
				v1 <= 2'b00;
				v2 <= 1'b1;
				if (main_counter == THDS) begin
					state <= 4'd3;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd3:
			begin
				v1 <= 2'b00;
				v2 <= 1'b1;
				shut <= 1'b1;
				if (main_counter == TS) begin
					state <= 4'd4;
					shut_load <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd4:
			begin
				v1 <= 2'b00;
				v2 <= 1'b1;
				shut <= 1'b0;
				shut_load <= 1'b0;
				if (shut_done) begin
					state <= 4'd5;
					main_counter <= 1'b0;
				end
			end
			4'd5:
			begin
				v1 <= 2'b01;
				v2 <= 1'b1;
				if (main_counter == T3P) begin
					state <= 4'd6;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd6:
			begin
				v1 <= 2'b10;
				v2 <= 0;
				if (main_counter == TV3RD) begin
					state <= 4'd7;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd7:
			begin
				v1 <= 2'b01;
				v2 <= 1;
				if (main_counter == T3D) begin
					state <= 4'd8;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd8:
			begin
				v1 <= 2'b00;
				v2 <= 1;
				if (main_counter == TD1L) begin
					state <= 4'd9;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd9:
			begin
				v1 <= 2'b01;
				v2 <= 0;
				vact <= 0;
				if (main_counter == TVCCD) begin
					state <= 4'd10;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd10:
			begin
				v1 <= 2'b00;
				v2 <= 1;
				vact <= 0;
				if (main_counter == THD) begin
					state <= 4'd11;
					main_counter <= 1'b0;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd11:
			begin
				v1 <= 2'b00;
				v2 <= 1;
				vact <= 1;
				if (main_counter == TACT) begin
					main_counter <= 1'b0;
					if (line_counter < LINES) begin
						state <= 4'd12;
						line_counter <= line_counter + 1'b1;
					end
					else begin
						state <= 4'd0;
						done <= 1'b1;
					end
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			4'd12:
			begin
				vact <= 0;
				if (main_counter == THL) begin
					main_counter <= 1'b0;
					state <= 4'd9;
				end
				else begin
					main_counter <= main_counter + 1'b1;
				end
			end
			default:
			begin
			   state <= 4'd0;
			end
		endcase
	end
end

endmodule
