`timescale 1ps/1ps

//
// ppg_v1r_tb.v: DG412 Deadtime Control TB
//
// Copyright 2024 Anhang Li <thelithcore@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

module ppg_v1r_tb;

    parameter SIM_CLK_FAST_FREQ   = 336e6;
    parameter SIM_CLK_HALF_PERIOD = 1.0/1e-12/(2*SIM_CLK_FAST_FREQ);
    
    always begin
        #SIM_CLK_HALF_PERIOD clk_fast = ~clk_fast;
    end

    reg clk_fast;
    reg rstn;
    reg trig_l;
    reg trig_h;
    reg v3mode;
    reg arm;
    reg [2:0] tdt;

    initial begin
        $dumpfile("ppg_v1r_tb.vcd");
        $dumpvars(0, ppg_v1r_tb);
        clk_fast = 0;
        rstn = 0;
        trig_l = 0;
        trig_h = 0;
        tdt  = 3'b111;

        // Test 1: Non-V3 Mode, Not Armed
        arm  = 0;
        v3mode = 0;
        #(2*SIM_CLK_HALF_PERIOD);
        rstn = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_l = 1;
        #(2*2*SIM_CLK_HALF_PERIOD);
        trig_l = 0;
        #(10000*2*SIM_CLK_HALF_PERIOD);

        // Test 2: Non-V3 Mode, Armed
        arm = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_l = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_l = 0;
        #(10000*2*SIM_CLK_HALF_PERIOD);
        trig_l = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_l = 0;
        #(10000*2*SIM_CLK_HALF_PERIOD);

        // Test 3: V3 Mode, Armed
        v3mode = 1;
        trig_l  = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_l = 0;
        #(40320*2*SIM_CLK_HALF_PERIOD);
        trig_h = 1;
        #(2*SIM_CLK_HALF_PERIOD);
        trig_h = 0;
        #(20000*2*SIM_CLK_HALF_PERIOD);

        $finish;
    end

    wire q_sell, q_pdn, q_plo, q_selh, q_phi;

    wire q_selhh, q_selhm, q_selhl;
    assign q_selhh = q_selh;
    assign q_selhm = ~q_selhl & ~q_selh;
    assign q_selhl = ~q_plo | ~v3mode;

    /*
    ppg_v1r u_dut(
        .clk_fast(clk_fast),
        .rstn(rstn),
        .trig_l(trig_l),
        .t_pdn_lead(13'd20),
        .t_pdn_hold(13'd1816),
        .t_plo_lead(13'd340),
        .t_plo_hold(13'd1176),
        .t_sell0_lead(13'd30),
        .t_sell0_hold(13'd300),
        .t_sell1_lead(13'd1526),
        .t_sell1_hold(13'd300),
        .q_sell(q_sell),
        .q_pdn (q_pdn),
        .q_plo (q_plo)
    );
    */
    ppg_v2r #(.CAPTURE(1)) u_dut(
        .clk_fast(clk_fast),
        .rstn(rstn),
        .trig_l(trig_l),
        .trig_h(trig_h),
        .v3mode(v3mode),    
        .t_pdn0_lead    (13'd0),
        .t_pdn0_hold    (13'd1556),
        .t_sell0_lead   (13'd20),
        .t_sell0_hold   (13'd300),
        .t_plo0_lead    (13'd340),
        .t_plo0_hold    (13'd876),
        .t_selh0_lead   (13'd20),
        .t_selh0_hold   (v3mode?13'd300:13'd0),
        .t_phi_lead     (13'd340),
        .t_phi_hold     (v3mode?13'd3020:13'd0),
        .t_selh1_lead   (13'd3380),
        .t_selh1_hold   (v3mode?13'd300:13'd0),
        .t_plo1_lead    (13'd3700),
        .t_plo1_hold    (v3mode?13'd6380:13'd0),
        .t_sell1_lead   (v3mode?13'd6720:13'd1236),
        .t_sell1_hold   (13'd300),
        .t_pdn1_lead    (13'd0),
        .t_pdn1_hold    (v3mode?13'd7040:13'd0),

        .q_sell(q_sell),
        .q_pdn (q_pdn),
        .q_plo (q_plo),
        .q_selh(q_selh),
        .q_phi (q_phi)
    );

    // Output Signals
    wire dg412_sell_p, dg412_sell_n;
    wire dg412_pdn_p, dg412_pdn_n;
    wire dg412_plo_p, dg412_plo_n;
    wire dg412_phi_p, dg412_phi_n;
    wire dg412_selh_h, dg412_selh_m, dg412_selh_l;

    wire [5:0] armed_v;
    wire armed_all = &armed_v;
    // DG412 Drivers for V2
    ppg_dg412_drive u_sell_drive(
        .clk_fast,.rstn,.arm,.armed(armed_v[0]),.tdt,
        .cki(q_sell),.ckop(dg412_sell_p),.ckon(dg412_sell_n)
    );

    ppg_dg412_drive u_pdn_drive(
        .clk_fast,.rstn,.arm,.armed(armed_v[1]),.tdt,
        .cki(q_pdn),.ckop(dg412_pdn_p),.ckon(dg412_pdn_n)
    );

    ppg_dg412_drive u_plo_drive(
        .clk_fast,.rstn,.arm,.armed(armed_v[2]),.tdt,
        .cki(q_plo),.ckop(dg412_plo_p),.ckon(dg412_plo_n)
    );

    ppg_dg412_drive u_phi_drive(
        .clk_fast,.rstn,.arm,.armed(armed_v[3]),.tdt,
        .cki(q_phi),.ckop(dg412_phi_p),.ckon(dg412_phi_n)
    );

    wire   selhm_p, selhm_n, selhh_p, selhh_n;
    assign dg412_selh_h = selhh_p;
    assign dg412_selh_m = selhm_p & selhh_n;
    assign dg412_selh_l = selhm_n;

    ppg_dg412_drive u_selh_drive0(
        .clk_fast,.rstn,.arm,.armed(armed_v[4]),.tdt,
        .cki(~q_selhl),.ckop(selhm_p),.ckon(selhm_n)
    );

    ppg_dg412_drive u_selh_drive1(
        .clk_fast,.rstn,.arm,.armed(armed_v[5]),.tdt,
        .cki(q_selhh),.ckop(selhh_p),.ckon(selhh_n)
    );

endmodule /* ppg_v1r */