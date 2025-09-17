import os
import subprocess
from pathlib import Path
import sys

from concrete_design.llvm_ir_parser import LLVMIRParser   # or paste the class into your script

try:
    xilinx_hls = Path(os.environ["XILINX_HLS"])
    print(f"Using XILINX_HLS: {xilinx_hls}")
except KeyError:
    sys.exit("❌  XILINX_HLS is not defined in your environment.")

# for 2024.1 "/tools/Xilinx/Vitis_HLS/2024.1/lnx64/tools/clang/bin/opt"
# opt_bin = xilinx_hls / "lnx64" / "tools" / "clang" / "bin" / "opt" 
# for 2024.2 "/tools/Xilinx/Vitis/2024.2/lnx64/tools/clang-3.1/bin/opt"

opt_bin = xilinx_hls / "lnx64" / "tools" / "clang-3.1" / "bin" / "opt"
lib_dir = xilinx_hls / "lib" / "lnx64.o"


in_bc  = Path("../0_work_dir/0_concrete_design/build/solution/.autopilot/db/a.o.3.bc")
out_ll = Path("../0_work_dir/0_concrete_design/logs/a.o.3.bc.ll")
cmd = [str(opt_bin), "-S", str(in_bc), "-o", str(out_ll)]
env = os.environ.copy()
env["LD_LIBRARY_PATH"] = f"{lib_dir}:{env.get('LD_LIBRARY_PATH','')}"
subprocess.run(cmd, env=env, check=True)

p = LLVMIRParser(out_ll, "../0_work_dir/0_concrete_design/logs/input_c_hierarchy.json")      # path to .ll file
p.build_llvm_ir_hierarchy_from_top("FFT_TOP")
p.save_hierarchy_to_json("../0_work_dir/0_concrete_design/logs/llvm_ir_hierarchy.json")  # save hierarchy to JSON file
p.save_pretty_print_hierarchy_to_txt("../0_work_dir/0_concrete_design/logs/llvm_ir_hierarchy.txt")  # save pretty-printed hierarchy to text file



