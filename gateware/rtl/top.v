//
// top.v: FPGA top level
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

module top (
    // Fixed Zynq IO
    inout wire [14:0]   DDR_addr,
    inout wire [2:0]    DDR_ba,
    inout wire          DDR_cas_n,
    inout wire          DDR_ck_n,
    inout wire          DDR_ck_p,
    inout wire          DDR_cke,
    inout wire          DDR_cs_n,
    inout wire [3:0]    DDR_dm,
    inout wire [31:0]   DDR_dq,
    inout wire [3:0]    DDR_dqs_n,
    inout wire [3:0]    DDR_dqs_p,
    inout wire          DDR_odt,
    inout wire          DDR_ras_n,
    inout wire          DDR_reset_n,
    inout wire          DDR_we_n,
    inout wire          FIXED_IO_ddr_vrn,
    inout wire          FIXED_IO_ddr_vrp,
    inout wire [53:0]   FIXED_IO_mio,
    inout wire          FIXED_IO_ps_clk,
    inout wire          FIXED_IO_ps_porb,
    inout wire          FIXED_IO_ps_srstb,
    // AFE Control
    output wire         AFE_RST,
    output wire         AFE_SYNC,
    output wire         AFE_SDATA,
    output wire         AFE_SL,
    output wire         AFE_SCK,
    // AFE Data
    input wire [13:0]   DVP_D,
    output wire         DVP_HSYNC,
    output wire         DVP_VSYNC,
    input wire          DVP_PCLK,
    // MIPI Output
    output wire 		DSI_LP_CP,
    output wire			DSI_LP_CN,
    output wire [1:0]	DSI_LP_DP,
    output wire [1:0]	DSI_LP_DN,
    output wire 		DSI_HS_CP,
    output wire 		DSI_HS_CN,
    output wire [1:0]	DSI_HS_DP,
    output wire [1:0]	DSI_HS_DN,
    // TCON for CCD
    output wire         TCON_V1,
    output wire         TCON_V2,
    output wire         TCON_V23,
    output wire         TCON_FDG,
    output wire         TCON_STROBE,
    output wire         TCON_H1,
    output wire         TCON_H2,
    output wire         TCON_RG,
    // PWM for VAB voltage control
    output wire         VAB_PWM
);

    parameter AXI_DW = 64;
    parameter AXI_AW = 32;
    parameter AXI_IDW = 2;

    wire clk;
    wire clk_lcd;
    wire rst;
    wire rst_lcd;
    wire aresetn;

    wire apb_pwrite;
    wire [31:0] apb_pwdata;
    wire [31:0] apb_paddr;
    wire apb_penable;
    wire apb_psel;
    wire apb_pslverr = 1'b0;
    wire apb_pready;
    wire [31:0] apb_prdata;

    wire [AXI_IDW-1:0] axi_awid;
    wire [AXI_AW-1:0] axi_awaddr;
    wire [7:0] axi_awlen;
    wire [2:0] axi_awsize;
    wire [1:0] axi_awburst;
    wire [1:0] axi_awlock = 2'b00; // Normal access
    wire [3:0] axi_awcache = 4'b0111; // WB no-allocate
    wire [2:0] axi_awqos = 3'b000; // No QoS
    wire [2:0] axi_awprot = 3'b000; // Unpriviledged secure data access
    wire axi_awvalid;
    wire axi_awready;
    wire [AXI_DW-1:0] axi_wdata;
    wire [(AXI_DW/8)-1:0] axi_wstrb;
    wire axi_wlast;
    wire axi_wvalid;
    wire axi_wready;
    wire [AXI_IDW-1:0] axi_bid;
    wire [1:0] axi_bresp;
    wire axi_bvalid;
    wire axi_bready;
    wire [AXI_IDW-1:0] axi_arid;
    wire [AXI_AW-1:0] axi_araddr;
    wire [7:0] axi_arlen;
    wire [2:0] axi_arsize;
    wire [1:0] axi_arburst;
    wire [1:0] axi_arlock = 2'b00; // Normal access
    wire [3:0] axi_arcache = 4'b1011; // WB no-allocate
    wire [2:0] axi_arqos = 3'b000; // No QoS
    wire [2:0] axi_arprot = 3'b000; // Unpriviledged secure data access
    wire axi_arvalid;
    wire axi_arready;
    wire [AXI_IDW-1:0] axi_rid;
    wire [AXI_DW-1:0] axi_rdata;
    wire [1:0] axi_rresp;
    wire axi_rlast;
    wire axi_rvalid;
    wire axi_rready;
    
    // TODO: AXI4 does not have WID. If more than 1 masters are present in the system,
    // Consider add a AXI3-AXI4 converter in the block diagram
    wire [AXI_IDW-1:0] axi_wid = 'd0;

	// Clock, reset are provided by the Zynq PS
	blkdsn_wrapper ps_wrapper (
       	.APB_M_0_paddr(apb_paddr),
        .APB_M_0_penable(apb_penable),
        .APB_M_0_prdata(apb_prdata),
        .APB_M_0_pready(apb_pready),
        .APB_M_0_psel(apb_psel),
        .APB_M_0_pslverr(apb_pslverr),
        .APB_M_0_pwdata(apb_pwdata),
        .APB_M_0_pwrite(apb_pwrite),
        .DDR_addr(DDR_addr),
        .DDR_ba(DDR_ba),
        .DDR_cas_n(DDR_cas_n),
        .DDR_ck_n(DDR_ck_n),
        .DDR_ck_p(DDR_ck_p),
        .DDR_cke(DDR_cke),
        .DDR_cs_n(DDR_cs_n),
        .DDR_dm(DDR_dm),
        .DDR_dq(DDR_dq),
        .DDR_dqs_n(DDR_dqs_n),
        .DDR_dqs_p(DDR_dqs_p),
        .DDR_odt(DDR_odt),
        .DDR_ras_n(DDR_ras_n),
        .DDR_reset_n(DDR_reset_n),
        .DDR_we_n(DDR_we_n),
        .FCLK_CLK0(clk),
        .FCLK_CLK1(),
        .FIXED_IO_ddr_vrn(FIXED_IO_ddr_vrn),
        .FIXED_IO_ddr_vrp(FIXED_IO_ddr_vrp),
        .FIXED_IO_mio(FIXED_IO_mio),
        .FIXED_IO_ps_clk(FIXED_IO_ps_clk),
        .FIXED_IO_ps_porb(FIXED_IO_ps_porb),
        .FIXED_IO_ps_srstb(FIXED_IO_ps_srstb),
        .S_AXI_HP0_0_araddr(axi_araddr),
        .S_AXI_HP0_0_arburst(axi_arburst),
        .S_AXI_HP0_0_arcache(axi_arcache),
        .S_AXI_HP0_0_arid(axi_arid),
        .S_AXI_HP0_0_arlen(axi_arlen),
        .S_AXI_HP0_0_arlock(axi_arlock),
        .S_AXI_HP0_0_arprot(axi_arprot),
        .S_AXI_HP0_0_arqos(axi_arqos),
        .S_AXI_HP0_0_arready(axi_arready),
        .S_AXI_HP0_0_arsize(axi_arsize),
        .S_AXI_HP0_0_arvalid(axi_arvalid),
        .S_AXI_HP0_0_awaddr(axi_awaddr),
        .S_AXI_HP0_0_awburst(axi_awburst),
        .S_AXI_HP0_0_awcache(axi_awcache),
        .S_AXI_HP0_0_awid(axi_awid),
        .S_AXI_HP0_0_awlen(axi_awlen),
        .S_AXI_HP0_0_awlock(axi_awlock),
        .S_AXI_HP0_0_awprot(axi_awprot),
        .S_AXI_HP0_0_awqos(axi_awqos),
        .S_AXI_HP0_0_awready(axi_awready),
        .S_AXI_HP0_0_awsize(axi_awsize),
        .S_AXI_HP0_0_awvalid(axi_awvalid),
        .S_AXI_HP0_0_bid(axi_bid),
        .S_AXI_HP0_0_bready(axi_bready),
        .S_AXI_HP0_0_bresp(axi_bresp),
        .S_AXI_HP0_0_bvalid(axi_bvalid),
        .S_AXI_HP0_0_rdata(axi_rdata),
        .S_AXI_HP0_0_rid(axi_rid),
        .S_AXI_HP0_0_rlast(axi_rlast),
        .S_AXI_HP0_0_rready(axi_rready),
        .S_AXI_HP0_0_rresp(axi_rresp),
        .S_AXI_HP0_0_rvalid(axi_rvalid),
        .S_AXI_HP0_0_wdata(axi_wdata),
        .S_AXI_HP0_0_wid(axi_wid),
        .S_AXI_HP0_0_wlast(axi_wlast),
        .S_AXI_HP0_0_wready(axi_wready),
        .S_AXI_HP0_0_wstrb(axi_wstrb),
        .S_AXI_HP0_0_wvalid(axi_wvalid),
        .peripheral_aresetn(aresetn)
	);

    // Reset synchronizer
    xpm_cdc_sync_rst rst_sync(
        .dest_clk(clk),
        .src_rst(!aresetn),
        .dest_rst(rst)
    );

    wire dvp_pclk_sdr;
    wire dvp_rst;
    wire clk_ccd;
    wire rst_ccd;
    ccdclk (
        .clk(DVP_PCLK),
        .rst(!aresetn),
        .clk_2x(dvp_pclk_sdr),
        .rst_2x(dvp_rst),
        .clk_4x(clk_ccd),
        .rst_4x(rst_ccd),
        .locked()
    );

    wire [15:0]  dsi_hs_dat;
    wire         dsi_hsck_ten;
    wire         dsi_hsdat_ten;
    system #(
        .AXI_AW (AXI_AW),
        .AXI_DW(AXI_DW),
        .AXI_IDW(AXI_IDW)
    ) system (
        .clk(clk),
        .clk_lcd(clk_lcd),
        .clk_ccd_4x(clk_ccd),
        .clk_ccd_1x(DVP_PCLK),
        .rst(rst),
        .rst_lcd(rst_lcd),
        .rst_ccd(rst_ccd),
        .dvp_d(DVP_D),
        .dvp_hsync(DVP_HSYNC),
        .dvp_vsync(DVP_VSYNC),
        .dvp_pclk(dvp_pclk_sdr),
        .dvp_rst(dvp_rst),
        .afe_rst(AFE_RST),
        .afe_sync(AFE_SYNC),
        .afe_sdata(AFE_SDATA),
        .afe_sl(AFE_SL),
        .afe_sck(AFE_SCK),
        .dsi_lp_cp(DSI_LP_CP),
        .dsi_lp_cn(DSI_LP_CN),
        .dsi_lp_dp(DSI_LP_DP),
        .dsi_lp_dn(DSI_LP_DN),
        .dsi_hs_dat(dsi_hs_dat),
        .dsi_hsck_ten(dsi_hsck_ten),
        .dsi_hsdat_ten(dsi_hsdat_ten),
        .tcon_v1(TCON_V1),
        .tcon_v2(TCON_V2),
        .tcon_v23(TCON_V23),
        .tcon_fdg(TCON_FDG),
        .tcon_strobe(TCON_STROBE),
        .tcon_h1(TCON_H1),
        .tcon_h2(TCON_H2),
        .tcon_rg(TCON_RG),
        .vab_pwm(VAB_PWM),
        .s_apb_pwrite(apb_pwrite),
        .s_apb_pwdata(apb_pwdata),
        .s_apb_paddr(apb_paddr),
        .s_apb_penable(apb_penable),
        .s_apb_psel(apb_psel),
        .s_apb_pready(apb_pready),
        .s_apb_prdata(apb_prdata),
        .m_axi_awid(axi_awid),
        .m_axi_awaddr(axi_awaddr),
        .m_axi_awlen(axi_awlen),
        .m_axi_awsize(axi_awsize),
        .m_axi_awburst(axi_awburst),
        .m_axi_awvalid(axi_awvalid),
        .m_axi_awready(axi_awready),
        .m_axi_wdata(axi_wdata),
        .m_axi_wstrb(axi_wstrb),
        .m_axi_wlast(axi_wlast),
        .m_axi_wvalid(axi_wvalid),
        .m_axi_wready(axi_wready),
        .m_axi_bid(axi_bid),
        .m_axi_bresp(axi_bresp),
        .m_axi_bvalid(axi_bvalid),
        .m_axi_bready(axi_bready),
        .m_axi_arid(axi_arid),
        .m_axi_araddr(axi_araddr),
        .m_axi_arlen(axi_arlen),
        .m_axi_arsize(axi_arsize),
        .m_axi_arburst(axi_arburst),
        .m_axi_arvalid(axi_arvalid),
        .m_axi_arready(axi_arready),
        .m_axi_rid(axi_rid),
        .m_axi_rdata(axi_rdata),
        .m_axi_rresp(axi_rresp),
        .m_axi_rlast(axi_rlast),
        .m_axi_rvalid(axi_rvalid),
        .m_axi_rready(axi_rready)
    );

    wire mmcm_locked;
    mu_dphy_7series #(.LANES(2)) dphy (
        .clk(clk),
        .rst(rst),
        .locked(mmcm_locked),
        .clk_lcd(clk_lcd),
        .rst_lcd(rst_lcd),
        .hsck_ten(dsi_hsck_ten),
        .hsdat_ten(dsi_hsdat_ten),
        .dat(dsi_hs_dat),
        .dsi_hs_cp(DSI_HS_CP),
        .dsi_hs_cn(DSI_HS_CN),
        .dsi_hs_dp(DSI_HS_DP),
        .dsi_hs_dn(DSI_HS_DN)
    );
    
//    assign ila_probe[0] = mmcm_locked;
//    assign ila_probe[7] = TCON_V1;
//    assign ila_probe[6] = TCON_V2;
//    assign ila_probe[5] = TCON_V23;
//    assign ila_probe[4] = TCON_FDG;
//    assign ila_probe[3] = TCON_STROBE;
//    assign ila_probe[2] = TCON_H1;
//    assign ila_probe[1] = TCON_RG;
//    assign ila_probe = dsi_hs_dat;

endmodule

`default_nettype wire
