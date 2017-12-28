`timescale 1ns / 1ps

////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer:
//
// Create Date:   16:03:05 12/27/2017
// Design Name:   Top
// Module Name:   C:/Users/ZephRay/Documents/GitHub/ProjectUshi/Code/FPGA/ProjectUshi/testbench.v
// Project Name:  ProjectUshi
// Target Device:  
// Tool versions:  
// Description: 
//
// Verilog Test Fixture created by ISE for module: Top
//
// Dependencies:
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
////////////////////////////////////////////////////////////////////////////////

module testbench;

	// Inputs
	reg [13:0] AFE_DR;
	reg [13:0] AFE_DL;
	reg CPU_SCLK;
	reg CPU_MOSI;
	reg CPU_RST;
	reg CLK_24M;

	// Outputs
	wire AFE_DCLK;
	wire AFE_HD;
	wire AFE_PBLK;
	wire AFE_CLPOB;
	wire AFE_SHP;
	wire AFE_SHD;
	wire AFE_CLPDM;
	wire AFE_VD;
	wire CCD_HL;
	wire CCD_H1;
	wire CCD_H2;
	wire CCD_RG;
	wire CCD_FD;
	wire CCD_SHUT;
	wire CCD_nXV1;
	wire CCD_nXV2;
	wire CCD_nXSG2;
	wire CPU_PCLK;
	wire CPU_VSYNC;
	wire CPU_HSYNC;
	wire [13:0] CPU_VD;
	wire [3:0] LED;

	// Instantiate the Unit Under Test (UUT)
	Top uut (
		.AFE_DR(AFE_DR), 
		.AFE_DL(AFE_DL), 
		.AFE_DCLK(AFE_DCLK), 
		.AFE_HD(AFE_HD), 
		.AFE_PBLK(AFE_PBLK), 
		.AFE_CLPOB(AFE_CLPOB), 
		.AFE_SHP(AFE_SHP), 
		.AFE_SHD(AFE_SHD), 
		.AFE_CLPDM(AFE_CLPDM), 
		.AFE_VD(AFE_VD), 
		.CCD_HL(CCD_HL), 
		.CCD_H1(CCD_H1), 
		.CCD_H2(CCD_H2), 
		.CCD_RG(CCD_RG), 
		.CCD_FD(CCD_FD), 
		.CCD_SHUT(CCD_SHUT), 
		.CCD_nXV1(CCD_nXV1), 
		.CCD_nXV2(CCD_nXV2), 
		.CCD_nXSG2(CCD_nXSG2), 
		.CPU_SCLK(CPU_SCLK), 
		.CPU_MOSI(CPU_MOSI), 
		.CPU_RST(CPU_RST), 
		.CPU_PCLK(CPU_PCLK), 
		.CPU_VSYNC(CPU_VSYNC), 
		.CPU_HSYNC(CPU_HSYNC), 
		.CPU_VD(CPU_VD), 
		.CLK_24M(CLK_24M), 
		.LED(LED)
	);

	initial begin
		// Initialize Inputs
		AFE_DR = 0;
		AFE_DL = 0;
		CPU_SCLK = 0;
		CPU_MOSI = 0;
		CPU_RST = 0;
		CLK_24M = 0;

		// Wait 100 ns for global reset to finish
		#100;
        
		// Add stimulus here

	end
      
endmodule

