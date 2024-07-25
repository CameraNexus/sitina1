#!/bin/bash

# Compile the Verilog code
iverilog -o ppg_v1r_tb ppg_v1r_tb.v ../ppg_dg412_drive.v ../ppg_v1r.v ../ppg_unit.v ../ppg_v2r.v

# Run the simulation
vvp ppg_v1r_tb

# Generate the VCD file
gtkwave ppg_v1r_tb.vcd ppg_v1r_tb.gtkw