//
// ccdtimgen.v: CCD Timing Generator
//
// Copyright 2024 Wenting Zhang <zephray@outlook.com>
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
`default_nettype none
`timescale 1ns / 1ps

module ccdtimgen(
    input  wire         clk,
    input  wire         clk_ccd, // 4X pixel clock
    input  wire         rst,
    input  wire         rst_ccd,
    // APB device port for register access
    `APB_SLAVE_IF,
    // To AFE
    output reg          dvp_hsync,
    output reg          dvp_vsync,
    // To CCD driver
    output reg          tcon_v1,
    output reg          tcon_v2,
    output reg          tcon_v23,
    output reg          tcon_fdg,
    output reg          tcon_strobe,
    output reg          tcon_h1,
    output reg          tcon_h2,
    output reg          tcon_rg
);

    // We run the CCD at 28MHz, or around 36ns pixel time
    // This logic runs at 4x CCD clock (112MHz, 9ns) to provide subpixel timing
    // Minimum pulse possible is then 9ns, used for RG

    localparam CNTW = 15; // Width of counters

    // These should be registers, just wires for now
    wire tgen_en = 1'b1;
    wire tgen_rst = 1'b0;
    wire tgen_embed_eshut = 1'b1;
    wire tgen_start_eshut = 1'b0;
    wire [CNTW-1:0] delay_htime = 10; // Start delay line time 
    wire [CNTW-1:0] delay_vtime = 10; // Start delay line count
    wire [3:0] vskip = 0; // Line skipping factor
    wire [CNTW-1:0] eshut_line = 100;

    // These can be synthesize-time constants
    // See pg. 26 for values, comment labed as min-nom-max
    // See fig. 17 for timing diagram
    wire [CNTW-1:0] t3p = 13332; // 100-120-200 us
    wire [CNTW-1:0] tv3rd = 1112; // 8-10-15 us
    wire [CNTW-1:0] t3d = 2224 ; // 15-20-80 us
    wire [CNTW-1:0] tl = 2040 * 4;
    wire [CNTW-1:0] tve = 4; // 0-na-100 ns
    wire [CNTW-1:0] tvccd = 392; // 3-3.5-20 us
    wire [CNTW-1:0] thd = 392; // 3-3.5-10 us
    wire [CNTW-1:0] th = 4; // TH is always 1 pixel time
    // See fig. 29 for timing diagram
    wire [CNTW-1:0] tfd = 196; // 0.5-na-na us
    // See fig. 30 for timing diagram
    wire [CNTW-1:0] ts = 444; // 3-4-10 us
    wire [CNTW-1:0] tsd = 168; // 1-1.5-10 us
    // Resolution settings
    wire [CNTW-3:0] hpix = 2040 - 1; // Total horizontal pixel cycles
    wire [CNTW-1:0] vpix = 2721 - 1;
    wire [CNTW-1:0] hsw = 2;

    // T3P + TV3RD + TL should be dividable by 4
    // TVCCD + THD should be diviable by 4
    // TL should be dividable by 4
    // TFD should be dividable by 4

    reg [CNTW-1:0] h_cnt;
    reg [CNTW-1:0] v_cnt;
    reg [3:0] v_cnt_sub; // Counter for line skipping

    reg [2:0] state;
    localparam STATE_IDLE = 3'd0;
    localparam STATE_DELAY = 3'd1;
    localparam STATE_1STLINE = 3'd2; // V2 pulse start
    localparam STATE_HFP = 3'd3;
    localparam STATE_ACTIVE = 3'd4;
    localparam STATE_HBP = 3'd5;
    localparam STATE_ESHUT = 3'd6;
    localparam STATE_LINESKIP = 3'd7;

    always @(posedge clk_ccd) begin
        case (state)
        STATE_IDLE: begin
            // Set idle state
            h_cnt <= 'd0;
            v_cnt <= 'd0;

            dvp_hsync <= 1'b0;
            dvp_vsync <= 1'b0;

            if (tgen_en) begin
                if (tgen_start_eshut) begin
                    // Enter delay state
                    state <= STATE_ESHUT;
                end
                else begin
                    // Starts shift out directly
                    state <= STATE_1STLINE;
                end
            end
        end
        STATE_DELAY: begin
            h_cnt <= h_cnt + 'd1;
            if (h_cnt == delay_htime) begin
                h_cnt <= 'd0;
                v_cnt <= v_cnt + 'd1;
                if (v_cnt == delay_vtime) begin
                    v_cnt <= 'd0;
                    state <= STATE_1STLINE;
                end
            end
        end
        STATE_1STLINE: begin
            // This stage only happens during the start of a frame
            // The V2 line is pulsed to transfer pixels from photosensitive
            // region to storage region for readout
            h_cnt <= h_cnt + 'd1;
            if (h_cnt == (t3p + tv3rd + tl - 'd1)) begin
                h_cnt <= 'd0;
                state <= STATE_HFP;
                dvp_vsync <= 1'b1;
            end
        end
        STATE_HFP: begin
            h_cnt <= h_cnt + 'd1;
            dvp_hsync <= (h_cnt < hsw);
            if (h_cnt == (tvccd + thd - 'd1)) begin
                h_cnt <= 'd0;
                if (tgen_embed_eshut && (v_cnt == eshut_line))
                    state <= STATE_ESHUT;
                else
                    state <= STATE_ACTIVE;
            end
        end
        STATE_ACTIVE: begin
            // Stage for pixel shift out
            h_cnt <= h_cnt + 'd1;
            if (h_cnt == {hpix, 2'b11}) begin
                h_cnt <= 'd0;
                state <= STATE_HBP;
            end
        end
        STATE_HBP: begin
            h_cnt <= h_cnt + 'd1;
            if (h_cnt == (th - 'd1)) begin
                h_cnt <= 'd0;
                v_cnt <= v_cnt + 'd1;
                if (v_cnt < vpix) begin
                    if (vskip != 'd0)
                        state <= STATE_LINESKIP;
                    else
                        state <= STATE_HFP;
                    dvp_vsync <= 1'b0;
                end
                else begin
                    if (tgen_en) begin
                        state <= STATE_1STLINE;
                    end
                end
            end
        end
        STATE_ESHUT: begin
            h_cnt <= h_cnt + 'd1;
            if (h_cnt == (ts + tsd - 'd1)) begin
                h_cnt <= 'd0;
                if (tgen_start_eshut)
                    // This eshut is applied at the start of the frame
                    state <= STATE_DELAY;
                else
                    state <= STATE_ACTIVE;
            end
        end
        STATE_LINESKIP: begin
            // Line skipping is a bit involved... it needs to wait additional TFD time
            if (v_cnt_sub == 'd0) begin
                // First line is just TFD
                if (h_cnt == (tfd - 'd1)) begin
                    h_cnt <= 'd0;
                    v_cnt_sub <= v_cnt_sub + 'd1;
                end
            end
            else begin
                // Normal counting
                if (h_cnt == (tvccd + thd - 'd1)) begin
                    v_cnt_sub <= v_cnt_sub + 'd1;
                    if (v_cnt_sub == vskip) begin
                        v_cnt_sub <= 'd0;
                        state <= STATE_1STLINE;
                    end
                    v_cnt <= v_cnt + 'd1;
                end
            end

            // H counter always counts
            h_cnt <= h_cnt + 'd1;
        end
        endcase

        if (rst_ccd) begin
            state <= STATE_IDLE;
            h_cnt <= 'd0;
            v_cnt <= 'd0;
            dvp_hsync <= 1'b0;
            dvp_vsync <= 1'b0;
        end
    end

    reg v1, v2, v23, fdg, strobe, h1, h2, rg;
    always @(*) begin
        // Set default inactive value
        v1 = 1'b1;
        v2 = 1'b0;
        v23 = 1'b0;
        fdg = 1'b0;
        h1 = 1'b1;
        h2 = 1'b0;
        rg = 1'b0;
        strobe = 1'b0;

        case (state)
        STATE_DELAY: begin
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_1STLINE: begin
            v1 = !((h_cnt >= t3p) && (h_cnt < (t3p + tv3rd - tve)));
            v2 = (h_cnt < (t3p + tv3rd + t3d));
            v23 = ((h_cnt >= (t3p - tve)) && (h_cnt < (t3p + tv3rd)));
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_HFP: begin
            v1 = !((h_cnt >= tve) && (h_cnt < (tvccd - tve)));
            v2 = h_cnt < tvccd;
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_ACTIVE: begin
            h1 = (h_cnt[0] == 1'b0);
            h2 = (h_cnt[0] == 1'b1);
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_HBP: begin
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_ESHUT: begin
            strobe = (h_cnt < ts);
            rg = (h_cnt[1:0] == 2'b00);
        end
        STATE_LINESKIP: begin
            if (v_cnt_sub == 'd0) begin
                fdg = 1'b1;
            end
            else begin
                v1 = !((h_cnt >= tve) && (h_cnt < (tvccd - tve)));
                v2 = h_cnt < tvccd;
                if (v_cnt_sub == vskip) begin
                    // Last line to skip
                    fdg = (h_cnt < (tvccd + thd - tfd));
                end
                else begin
                    fdg = 1'b1;
                end
            end
            rg = (h_cnt[1:0] == 2'b00);
        end
        default: ;
        endcase
    end

    always @(posedge clk) begin
        tcon_v1 <= v1;
        tcon_v2 <= v2;
        tcon_v23 <= v23;
        tcon_fdg <= fdg;
        tcon_h1 <= h1;
        tcon_h2 <= h2;
        tcon_rg <= rg;
        tcon_strobe <= strobe;

        if (rst_ccd) begin
            tcon_v1 <= 1'b1;
            tcon_v2 <= 1'b0;
            tcon_v23 <= 1'b0;
            tcon_fdg <= 1'b0;
            tcon_h1 <= 1'b1;
            tcon_h2 <= 1'b0;
            tcon_rg <= 1'b0;
            tcon_strobe <= 1'b0;
        end
    end

    wire tgen_running = state != STATE_IDLE;

endmodule

`default_nettype wire
