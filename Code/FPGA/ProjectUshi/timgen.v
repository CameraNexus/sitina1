`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// ProjectUshi Verilog Code
// Copyright 2018 Wenting Zhang 
//
// Create Date:    10:18:23 12/14/2017 
// Module Name:    timgen 
// Description:    CCD & AFE timing generator
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module timgen(
    input clk_pix, //Pixel clock
	 input clk_rg,  //RG clock
    input rst,
	 input trigger,
    output done,
	 output afe_dclk,
    output afe_hd,
    output afe_pblk,
    output afe_clpob,
    output afe_shp,
    output afe_shd,
    output afe_clpdm,
    output afe_vd,
    output ccd_hl,
    output ccd_h1,
    output ccd_h2,
    output ccd_rg,
    output ccd_fd,
    output ccd_shut,
	 output [1:0] ccd_v1, //2'b00 Low 2'b01 Mid 2'b10 High
	 output ccd_v2, //V2 refers to V2 on CCD
	 output cpu_pclk,
    output cpu_vsync,
    output cpu_hsync
    );

assign ccd_h1 = !ccd_h2;
assign ccd_hl = ccd_h2;

wire vact;

tim_vfsm tim_vfsm(
    .clk(clk_pix), 
    .rst(rst),
    .trigger(trigger),
    .a(), 
    .d(), 
	 .we(),
    .v1(ccd_v1),
    .v2(ccd_v2), 
    .shut(ccd_shut),
    .vact(vact) 
    );

endmodule
