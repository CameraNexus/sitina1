`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   07:40:09 12/27/2017
// Design Name:   timgen
// Module Name:   C:/Users/ZephRay/Documents/GitHub/ProjectUshi/Code/FPGA/ProjectUshi/timgen_test.v
// Project Name:  ProjectUshi
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: timgen
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module timgen_test;

	// Inputs
	wire clk_pix;
	wire clk_rg;
	reg rst;
	reg trigger;

	// Outputs
	wire done;
	wire afe_dclk;
	wire afe_hd;
	wire afe_pblk;
	wire afe_clpob;
	wire afe_shp;
	wire afe_shd;
	wire afe_clpdm;
	wire afe_vd;
	wire ccd_hl;
	wire ccd_h1;
	wire ccd_h2;
	wire ccd_rg;
	wire ccd_fd;
	wire ccd_shut;
	wire [1:0] ccd_v1;
	wire ccd_v2;
	wire cpu_pclk;
	wire cpu_vsync;
	wire cpu_hsync;

	// Instantiate the Unit Under Test (UUT)
	timgen uut (
		.clk_pix(clk_pix), 
		.clk_rg(clk_rg), 
		.rst(rst), 
		.trigger(trigger), 
		.done(done), 
		.afe_dclk(afe_dclk), 
		.afe_hd(afe_hd), 
		.afe_pblk(afe_pblk), 
		.afe_clpob(afe_clpob), 
		.afe_shp(afe_shp), 
		.afe_shd(afe_shd), 
		.afe_clpdm(afe_clpdm), 
		.afe_vd(afe_vd), 
		.ccd_hl(ccd_hl), 
		.ccd_h1(ccd_h1), 
		.ccd_h2(ccd_h2), 
		.ccd_rg(ccd_rg), 
		.ccd_fd(ccd_fd), 
		.ccd_shut(ccd_shut), 
		.ccd_v1(ccd_v1), 
		.ccd_v2(ccd_v2), 
		.cpu_vsync(cpu_vsync), 
		.cpu_hsync(cpu_hsync)
	);
	
	clk_gen #(33.333) clk_gen_30m(clk_pix);
	clk_gen #(3.333) clk_gen_300m(clk_rg);

	initial begin
		// Initialize Inputs
		rst = 0;
		trigger = 0;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here
		rst = 1;
		#100;
		rst = 0;
		trigger = 1;
		
	end
      
endmodule

