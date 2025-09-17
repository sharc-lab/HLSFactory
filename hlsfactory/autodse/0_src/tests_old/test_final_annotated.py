#!/usr/bin/env python3

from concrete_design.c_to_rtl_mapper import CToRTLMapper

# Create mapper and run full analysis with parent mapping
mapper = CToRTLMapper("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_9/logs")

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
    
    # Generate final validation and annotated results
    unmapped, missing = mapper.validate_against_csynth()
    
    # Create final annotated data with all mappings
    annotated_data, unmapped_modules = mapper.annotate_csynth_with_c_info(
        "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_9/logs/rtl_c_annotated_complete.json"
    )
    
    print("=== FINAL RESULTS ===")
    print(f"Total modules: {annotated_data['summary']['total_modules']}")
    print(f"Mapped modules: {annotated_data['summary']['mapped_modules']}")
    print(f"Unmapped modules: {annotated_data['summary']['unmapped_modules']}")
    
    if unmapped_modules:
        print("\nRemaining unmapped modules:")
        for module in unmapped_modules:
            print(f"  - {module}")
    else:
        print("\n🎉 All modules successfully mapped!")

    # Show parent mapping examples
    print("\n=== PARENT MAPPED EXAMPLES ===")
    for module_name, module_data in annotated_data["modules"].items():
        if (module_data.get("c_source") and 
            module_data["c_source"].get("mapping_type") == "merge_unmapped_to_parent"):
            print(f"{module_name}: mapped to parent {module_data['c_source']['c_function']}")