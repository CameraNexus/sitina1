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
    inout [13:0] CPU_VD,
    input CLK_24M,
    output [3:0] LED
    );

wire clk_300m;
wire clk_30m;
wire clk_60m;
wire pll_locked;

pll1 pll_1
   (// Clock in ports
    .CLK_IN1(CLK_24M),      // IN
    // Clock out ports
    .CLK_OUT1(clk_300m),     // OUT
    .CLK_OUT2(clk_30m),     // OUT
	 .CLK_OUT3(clk_60m),
    // Status and control signals
    .LOCKED(pll_locked));      // OUT
	 
wire[1:0] ccd_v1;
wire ccd_v2;

assign CCD_nXV1 = ccd_v2;
assign CCD_nXSG2 = ccd_v1[1];
assign CCD_nXV2 = ((ccd_v1 == 2'b10) || (ccd_v1 == 2'b01)) ? 1'b1 : 1'b0;

wire trigger;
assign trigger = 1;

wire rst;
assign rst = !(CPU_RST & pll_locked);

wire done;

timgen timgen 
	(
    .clk_pix(clk_30m),
	 .clk_rg(clk_300m),
    .rst(rst),
	 .trigger(trigger),
    .done(done),
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
    .cpu_vsync(CPU_VSYNC),
    .cpu_hsync(CPU_HSYNC)
    );

reg [23:0] led_counter;

always @(posedge clk_30m)
begin
  led_counter <= led_counter + 1'b1;
end

assign LED[3] = led_counter[23];
assign LED[2] = rst;
assign LED[1] = trigger;
assign LED[0] = done;

assign CPU_VD[13:6] = led_counter[7:0];
assign CPU_VD[5:0] = 6'bz;

ODDR2 #(
    .DDR_ALIGNMENT("NONE"),
    .INIT(1'b0),
    .SRTYPE("SYNC")
    ) clk_forward
(
      .Q(CPU_PCLK), 
      .C0(clk_60m), 
      .C1(~clk_60m),
      .CE(1'b1),
      .D0(1'b1),
      .D1(1'b0),
      .R(1'b0),  
      .S(1'b0) 
);

endmodule
