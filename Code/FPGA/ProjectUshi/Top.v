`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// ProjectUshi Verilog Code
// Copyright 2018 Wenting Zhang 
//
// Create Date:    18:21:32 12/13/2017 
// Module Name:    Top  
// Description:    Top layer implementation
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module Top(
    input [13:0] AFE_DR,
    input [13:0] AFE_DL,
    output AFE_DCLK,
    output AFE_HD,
    output AFE_PBLK,
    output AFE_CLPOB,
    output AFE_SHP,
    output AFE_SHD,
    output AFE_CLPDM,
    output AFE_VD,
    output CCD_HL,
    output CCD_H1,
    output CCD_H2,
    output CCD_RG,
    output CCD_FD,
    output CCD_SHUT,
	 output CCD_nXV1, 
	 output CCD_nXV2,
	 output CCD_nXSG2,
    input CPU_SCLK,
    input CPU_MOSI,
    input CPU_RST,
    output CPU_PCLK,
    output CPU_VSYNC,
    output CPU_HSYNC,
    output [13:0] CPU_VD,
    input CLK_60M,
    output [3:0] LED
    );

wire CLK_300M;
wire CLK_30M;

wire PLL_RESET;
wire PLL_LOCKED;

pll1 pll_1
   (// Clock in ports
    .CLK_IN1(CLK_60M),      // IN
    // Clock out ports
    .CLK_OUT1(CLK_300M),     // OUT
    .CLK_OUT2(CLK_30M),     // OUT
    // Status and control signals
    .LOCKED(LOCKED));      // OUT
	 
wire[1:0] ccd_v1;
wire ccd_v2;

assign CCD_nXV1 = ccd_v2;
assign CCD_nXSG2 = ccd_v1[1];
assign CCD_nXV2 = ((ccd_v1 == 2'b10) || (ccd_v1 == 2'b01)) ? 1'b1 : 1'b0;

wire trigger;
assign trigger = 1;

timgen timgen 
	(
    .clk_pix(CLK_30M),
	 .clk_rg(CLK_300M),
    .rst(CPU_RST),
	 .trigger(trigger),
    .done(),
	 .afe_dclk(AFE_DCLK),
    .afe_hd(AFE_HD),
    .afe_pblk(AFE_PBLK),
    .afe_clpob(AFE_CLPOB),
    .afe_shp(AFE_SHP),
    .afe_shd(AFE_SHD),
    .afe_clpdm(AFE_CLPDM),
    .afe_vd(AFE_VD),
    .ccd_hl(CCD_HL),
    .ccd_h1(CCD_H1),
    .ccd_h2(CCD_H2),
    .ccd_rg(CCD_RG),
    .ccd_fd(CCD_FD),
    .ccd_shut(CCD_SHUT),
	 .ccd_v1(ccd_v1), 
	 .ccd_v2(ccd_v2),
	 .cpu_pclk(CPU_PCLK),
    .cpu_vsync(CPU_VSYNC),
    .cpu_hsync(CPU_HSYNC)
    );


endmodule
