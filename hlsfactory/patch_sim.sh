#!/bin/bash -e

rm -rf sim.generated
cp -r sim sim.generated

# FIXME: this entire thing should be done with a proper Verilog parser

# change name of AUTOTB_DUT to bd_0_wrapper
sed -i 's/`define AUTOTB_DUT .*/`define AUTOTB_DUT bd_0_wrapper/' sim/verilog/*.autotb.v

# make AXI signals lowercase
for signal_type in AWVALID AWREADY AWADDR AWLEN AWSIZE AWBURST AWLOCK AWCACHE AWPROT AWQOS AWREGION WVALID WREADY WDATA WSTRB WLAST ARVALID ARREADY ARADDR ARLEN ARSIZE ARBURST ARLOCK ARCACHE ARPROT ARQOS ARREGION RVALID RREADY RDATA RLAST RRESP BVALID BREADY BRESP; do
    sed -i "s/\.\([ms]_axi_\w*\)_$signal_type(/.\1_$(tr '[[:upper:]]' '[[:lower:]]' <<<"$signal_type")(/" sim/verilog/*.autotb.v
done

# remove AXI signals that are not used
for signal_type in AWID AWUSER WID WUSER ARID ARUSER RID RUSER BID BUSER; do
    sed -i "/\.[ms]_axi_\w*_$signal_type(/d" sim/verilog/*.autotb.v
done

# make relative paths absolute (for test vector data files)
sed -i 's/"\.\.\//"'"$(sed 's/\//\\\//g' <<<"$PWD")"'\/sim\//' sim/verilog/*.v

# remove dataflow_monitor
sed -i '/^dataflow_monitor /{:a;N;/;/!ba};//d' sim/verilog/*.autotb.v  # https://stackoverflow.com/a/37681075/5233414

# TODO: fixup syntax at end of `AUTOTB_DUT_INST instantiation
