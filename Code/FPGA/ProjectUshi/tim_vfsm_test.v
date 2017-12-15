`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   19:39:32 12/14/2017
// Design Name:   tim_vfsm
// Module Name:   C:/Users/ZephRay/Documents/FPGA/ProjectUshi/tim_vfsm_test.v
// Project Name:  ProjectUshi
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: tim_vfsm
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module tim_vfsm_test;

	// Inputs
	wire clk;
	reg rst;
	reg trigger;
	reg [1:0] a;
	reg [7:0] d;
	reg we;

	// Outputs
	wire [1:0] v1;
	wire v2;
	wire shut;
	wire vact;

	// Instantiate the Unit Under Test (UUT)
	tim_vfsm uut (
		.clk(clk), 
		.rst(rst), 
		.trigger(trigger), 
		.a(a), 
		.d(d), 
		.we(we), 
		.v1(v1), 
		.v2(v2), 
		.shut(shut), 
		.vact(vact)
	);
	
	clk_gen clk_gen(clk);
	
	initial begin
		// Initialize Inputs
		rst = 0;
		trigger = 0;
		a = 0;
		d = 0;
		we = 0;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here
		rst = 1;
		#100;
		rst = 0;
		trigger = 1;

	end
      
endmodule

module clk_gen(output reg clk);

	parameter period = 33.333;
	
	initial clk = 0;
	
	always begin
		#(period/2);
		clk = ~clk;
	end
	
endmodule
