#!/usr/bin/env python3

from concrete_design.c_to_rtl_mapper import CToRTLMapper

# Create mapper
mapper = CToRTLMapper("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_0/logs")

# Load data
if mapper.load_json_files():
    mapper.build_c_hierarchy_index()
    
    print("=== C LOOPS DETECTED ===")
    for line, loop_info in mapper.c_loops_by_line.items():
        print(f"Line {line}: {loop_info}")
    
    print("=== LLVM MODULES WITH LOOPS ===")
    mapper.build_llvm_hierarchy_index()
    for module_name, module_info in mapper.llvm_modules.items():
        if "loops" in module_info and module_info["loops"]:
            print(f"{module_name}: {module_info['loops']}")
    
    print("=== SIMPLE MAPPING ATTEMPT ===")
    # Try simple mapping
    for module_name, module_info in mapper.llvm_modules.items():
        if "loops" in module_info and module_info["loops"]:
            loop_label = module_info["loops"][0]
            print(f"\nModule: {module_name}")
            print(f"  Loop label: {loop_label}")
            
            # Find the original C loop
            c_loop = None
            for line, loop_info in mapper.c_loops_by_line.items():
                if loop_info["label"] == loop_label:
                    c_loop = loop_info
                    break
            
            if c_loop:
                print(f"  ✓ Found C loop: {c_loop}")
            else:
                print(f"  ✗ No C loop found for '{loop_label}'")