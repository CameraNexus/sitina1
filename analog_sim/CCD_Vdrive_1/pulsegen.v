module pulsegen ( clk, reset, t1, t2, trig, q, qbar ) ;
   input reg clk;
   input reg reset;
   input integer unsigned t1;
   input integer unsigned t2;
   input reg trig;
   output reg q;
   output qbar;

   integer unsigned counter;
   always @* begin
      q = 0;
      if(counter>t1) q = 1;
      if(counter>t2) q = 0;
   end

   assign qbar = ~q;
   always @(posedge clk or posedge reset) begin
      if(reset)          counter <= 0;
      else if(trig)      counter <= 1;
      else if(counter>0) counter <= counter + 1;
   end

endmodule
