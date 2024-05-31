# 
# Usage: To re-create this platform project launch xsct with below options.
# xsct /home/wenting/workspace/platform/platform.tcl
# 
# OR launch xsct and run below command.
# source /home/wenting/workspace/platform/platform.tcl
# 
# To create the platform in a different location, modify the -out option of "platform create" command.
# -out option specifies the output directory of the platform project.

platform create -name {platform}\
-hw {/home/wenting/Sitina1/gateware/syn/top.xsa}\
-proc {ps7_cortexa9_0} -os {standalone} -out {/home/wenting/workspace}

platform write
platform generate -domains 
platform active {platform}
domain active {zynq_fsbl}
bsp reload
platform generate
platform active {platform}
platform config -updatehw {/home/wenting/Sitina1/gateware/syn/top.xsa}
platform config -updatehw {/home/wenting/Sitina1/gateware/syn/top.xsa}
platform generate
