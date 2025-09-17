#!/usr/bin/env python3

from concrete_design.llvm_ir_parser import LLVMIRParser

# Test LLVM IR parser with debug output
ll_file = "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_0/logs/a.o.3.bc.ll"
input_c_hierarchy = "input_c_hierarchy.json"

print("=== Testing LLVM IR Parser ===")
parser = LLVMIRParser(ll_file, input_c_hierarchy)
parser.build_llvm_ir_hierarchy_from_top("FFT_TOP")

print(f"\n=== Final loop_string mapping ===")
for gv, loop_name in parser.loop_string.items():
    print(f"@{gv} -> '{loop_name}'")

print(f"\n=== Functions and their loops ===")
for fn_name, fn_info in parser.functions.items():
    if fn_info.loop_headers:
        print(f"{fn_name}: {list(fn_info.loop_headers)}")