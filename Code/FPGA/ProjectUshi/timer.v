`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:42:31 12/14/2017 
// Design Name: 
// Module Name:    timer 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module timer(
    input clk,
    input load,
    input [WIDTH - 1:0] preset,
    output reg [WIDTH - 1:0] count,
    output reg done
    );

parameter WIDTH = 16;

reg[12:0] prediv;

always @(posedge clk)
begin
	if (load) begin
		count <= preset;
		done <= 0;
		prediv <= 0;
	end
	else begin
		if (count == 0) begin
			done <= 1;
			count <= 0;
		end
		else begin 
		   if (prediv == 13'd1875) begin
				prediv <= 13'b0;
				count <= count - 1'b1;
			end
			else begin
				prediv <= prediv + 1'b1;
			end
			done <= 0;
		end
	end
end

endmodule
