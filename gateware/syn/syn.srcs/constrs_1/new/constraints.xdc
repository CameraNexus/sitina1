
set_false_path -from [get_clocks clk_fpga_0] -to [get_clocks clk_fpga_1]
set_false_path -from [get_clocks clk_fpga_1] -to [get_clocks clk_fpga_0]

set_property PACKAGE_PIN Y19 [get_ports AFE_RST]
set_property PACKAGE_PIN W16 [get_ports AFE_SCK]
set_property PACKAGE_PIN Y17 [get_ports AFE_SDATA]
set_property PACKAGE_PIN Y18 [get_ports AFE_SL]
set_property PACKAGE_PIN T11 [get_ports AFE_SYNC]
set_property IOSTANDARD LVCMOS18 [get_ports AFE_RST]
set_property IOSTANDARD LVCMOS18 [get_ports AFE_SCK]
set_property IOSTANDARD LVCMOS18 [get_ports AFE_SDATA]
set_property IOSTANDARD LVCMOS18 [get_ports AFE_SL]
set_property IOSTANDARD LVCMOS18 [get_ports AFE_SYNC]

set_property PACKAGE_PIN U20 [get_ports {DSI_HS_DN[0]}]
set_property PACKAGE_PIN P20 [get_ports {DSI_HS_DN[1]}]
set_property PACKAGE_PIN W20 [get_ports DSI_HS_CN]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {DSI_HS_DP[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {DSI_HS_DN[0]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {DSI_HS_DP[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports {DSI_HS_DN[1]}]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports DSI_HS_CP]
set_property IOSTANDARD DIFF_HSTL_I_18 [get_ports DSI_HS_CN]

set_property PACKAGE_PIN W19 [get_ports DSI_LP_CN]
set_property IOSTANDARD LVCMOS18 [get_ports DSI_LP_CN]
set_property PACKAGE_PIN U19 [get_ports DSI_LP_CP]
set_property IOSTANDARD LVCMOS18 [get_ports DSI_LP_CP]
set_property PACKAGE_PIN N18 [get_ports {DSI_LP_DP[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {DSI_LP_DP[1]}]
set_property PACKAGE_PIN R19 [get_ports {DSI_LP_DP[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {DSI_LP_DP[0]}]
set_property PACKAGE_PIN T19 [get_ports {DSI_LP_DN[0]}]
set_property IOSTANDARD LVCMOS18 [get_ports {DSI_LP_DN[0]}]
set_property PACKAGE_PIN P19 [get_ports {DSI_LP_DN[1]}]
set_property IOSTANDARD LVCMOS18 [get_ports {DSI_LP_DN[1]}]