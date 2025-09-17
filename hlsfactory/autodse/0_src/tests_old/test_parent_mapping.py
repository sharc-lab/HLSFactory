#!/usr/bin/env python3

from concrete_design.c_to_rtl_mapper import CToRTLMapper
import json

# Create mapper and run full analysis
mapper = CToRTLMapper("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_0/logs")

if mapper.load_json_files():
    mapper.build_c_hierarchy_index()
    mapper.build_llvm_hierarchy_index()
    
    mapper.map_simple_cases()
    mapper.map_line_based_cases()
    mapper.map_outlined_cases()
    mapper.map_function_level_cases()
    mapper.map_flattened_cases()
    mapper.apply_legalization()
    mapper.map_unmapped_to_parent()
    
    print("=== PARENT MAPPED MODULES ===")
    for module_name, mapping in mapper.final_mappings.items():
        if mapping.get("type") == "merge_unmapped_to_parent":
            print(f"\nModule: {module_name}")
            print(json.dumps(mapping, indent=2))

    # Save updated mapping
    output_file = "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_0/logs/c_to_rtl_mapping_with_parent.json"
    mapper.export_mapping_data_structure(str(output_file))