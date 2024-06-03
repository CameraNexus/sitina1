
module controller ( clk, reset, trig, state, pd, pul, puh, sel, seh, lss ) ;
   input reg clk;
   input reg reset;
   input reg trig;
   output [2:0] state;
   output reg pd;
   output reg pul;
   output reg puh;
   output reg sel;
   output reg seh;
   output reg lss;

   reg [3:0] fsm;
   assign state = fsm[3:1];

   // Deadtime settings
   localparam TDS_L_LPE  = 3;
   localparam TDS_LPE_M1 = 3;
   localparam TDS_M1_HPE = 3;
   localparam TDS_HPE_H  = 3;
   localparam TDS_H_HNE  = 3;
   localparam TDS_HNE_M2 = 3;
   localparam TDS_M2_LNE = 3;
   localparam TDS_LNE_L  = 3;

   localparam T_LPE = 10;   // VDDL Posedge
   localparam T_M1  = 100;  // VDDL First Plateau
   localparam T_HPE = 10;   // VDDH Posedge
   localparam T_H   = 50;  // VDDH Duration
   localparam T_HNE = 10;   // VDDH Negedge
   localparam T_M2  = 100;  // VDDL Second Plateau
   localparam T_LNE = 10;   // VDDL Nedgedge

   always @(*) begin
      {pd,pul,puh,sel,seh,lss} = 0;
      if(~fsm[0]) begin  // The other states are Deadtime states
         case(state)
            3'd0: {pd,pul,puh,sel,seh,lss} = 6'b100000; // L
            3'd1: {pd,pul,puh,sel,seh,lss} = 6'b000100; // LPE
            3'd2: {pd,pul,puh,sel,seh,lss} = 6'b010000; // M1
            3'd3: {pd,pul,puh,sel,seh,lss} = 6'b000011; // HPE
            3'd4: {pd,pul,puh,sel,seh,lss} = 6'b001001; // H
            3'd5: {pd,pul,puh,sel,seh,lss} = 6'b000011; // HNE
            3'd6: {pd,pul,puh,sel,seh,lss} = 6'b010000; // M2
            3'd7: {pd,pul,puh,sel,seh,lss} = 6'b000100; // LNE
         endcase
      end
   end

   // Implement the module here
   integer counter;
   always @(posedge clk or posedge reset) begin
      if(reset) begin
         fsm     <= 0;
         counter <= 0;
      end else begin
         if (fsm==0) begin
            if(trig) fsm <= 1;
            counter <= 0;
         end else if (fsm==1) begin
            if(counter>=TDS_L_LPE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==2) begin
            if(counter>=T_LPE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==3) begin
            if(counter>=TDS_LPE_M1-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==4) begin
            if(counter>=T_M1-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==5) begin
            if(counter>=TDS_M1_HPE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==6) begin
            if(counter>=T_HPE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==7) begin
            if(counter>=TDS_HPE_H-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==8) begin
            if(counter>=T_H-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==9) begin
            if(counter>=TDS_H_HNE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==10) begin
            if(counter>=T_HNE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==11) begin
            if(counter>=TDS_HNE_M2-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==12) begin
            if(counter>=T_M2-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==13) begin
            if(counter>=TDS_M2_LNE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==14) begin
            if(counter>=T_LNE-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end else if (fsm==15) begin
            if(counter>=TDS_LNE_L-1) begin
               counter <= 0;
               fsm     <= fsm + 1;
            end else
               counter <= counter + 1;
         end
      end
   end

endmodule
