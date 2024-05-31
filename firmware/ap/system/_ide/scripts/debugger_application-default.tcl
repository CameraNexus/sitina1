# Usage with Vitis IDE:
# In Vitis IDE create a Single Application Debug launch configuration,
# change the debug type to 'Attach to running target' and provide this 
# tcl script in 'Execute Script' option.
# Path of this script: /home/wenting/workspace/system/_ide/scripts/debugger_application-default.tcl
# 
# 
# Usage with xsct:
# To debug using xsct, launch xsct and run below command
# source /home/wenting/workspace/system/_ide/scripts/debugger_application-default.tcl
# 
connect -url tcp:127.0.0.1:3121
targets -set -nocase -filter {name =~"APU*"}
rst -system
after 3000
targets -set -filter {jtag_cable_name =~ "Digilent JTAG-HS3 210299ACFCEF" && level==0 && jtag_device_ctx=="jsn-JTAG-HS3-210299ACFCEF-13722093-0"}
fpga -file /home/wenting/Sitina1/gateware/syn/syn.runs/impl_1/top.bit
targets -set -nocase -filter {name =~"APU*"}
loadhw -hw /home/wenting/workspace/platform/export/platform/hw/top.xsa -mem-ranges [list {0x40000000 0xbfffffff}] -regs
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*"}
source /home/wenting/workspace/application/_ide/psinit/ps7_init.tcl
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "*A9*#0"}
dow /home/wenting/workspace/application/Debug/application.elf
configparams force-mem-access 0
bpadd -addr &main
