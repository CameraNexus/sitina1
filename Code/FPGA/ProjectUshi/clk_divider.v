`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:33:30 12/14/2017 
// Design Name: 
// Module Name:    clk_divider 
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
module clk_divider(
    input clk,
    input rst,
    output reg out
    );

parameter DIV = 10;

reg [31:0] count;

always @(posedge(clk), posedge(rst))
begin
	if (rst == 1)
		count <= 32'b0;
	else if (count == DIV - 1)
		count <= 32'b0;
	else
		count <= count + 1;
end

always @(posedge(clk), posedge(rst))
begin
	if (rst == 1)
		out <= 1'b0;
	else if (count == DIV - 1)
		out <= ~out;
	else
		out <= out;
end

endmodule
