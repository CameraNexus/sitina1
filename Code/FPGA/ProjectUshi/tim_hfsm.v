`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// ProjectUshi Verilog Code
// Copyright 2018 Wenting Zhang 
// 
// Create Date:    08:47:21 12/26/2017 
// Module Name:    tim_hfsm 
// Description:    Horizontal Timing FSM
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module tim_hfsm(
    input clk,
    input vact,
    output r,
    output h,
    output clpob,
    output clpdm,
    output pblk,
    output shp,
    output shd,
    output dclk
    );

localparam NDUM = 12;
localparam NBLA = 26;
localparam NBUF = 16;
localparam NACT = 2436;
localparam NPIX = NDUM + NBLA + NBUF + NACT;

reg[12:0] pixel_counter;
reg[3:0] state;

wire[3:0] next_state;

assign next_state = ((state == 4'd0)?((v_act == 1)?(4'd1):(4'd0)):((state == 4'd10)?(4'd0):(state + 1'b1)));

always @(posedge clk)
begin
  if (rst == 1) begin
    state <= 4'b0;
    pixel_counter <= 13'b0;
  end
  else begin
    state <= next_state;
    pixel_counter <= (pixel_counter == NPIX) ? (13'b0) : (pixel_counter + 1);
  end
end

assign h = (state >= 4'd1)&(state <=4'd5) ? 1 : 0;
assign r = (state == 4'd1) ? 1 : 0;
assign shp = (state >= 4'd1)&(state <= 4'd3) ? 0 : 1;
assign shd = (state >= 4'd6)&(state <= 4'd8) ? 0 : 1;
assign dclk = h;

assign clpob = (pixel_counter >= (NDUM + 1))&(pixel_counter <= (NDUM + NBLA - 1)) ? 0 : 1;
assign clpdm = clpob; 

endmodule
