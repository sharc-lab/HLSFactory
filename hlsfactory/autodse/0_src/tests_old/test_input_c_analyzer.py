
from concrete_design.input_c_analyzer import InputCAnalyzer

# Test on work_0 directory where STREAM_OUT_REVERSE should be found
analyzer = InputCAnalyzer("../0_work_dir/dse_run/work_0/FFT.cpp")
funcs = analyzer.generate_input_c_hierarchy()

print("=== DETECTED FUNCTIONS ===")
for func in funcs:
    print(f"Function: {func['name']} (lines {func['start_line']}-{func['end_line']})")
    if func.get('children'):
        print(f"  Children: {len(func['children'])}")
        for child in func['children']:
            if child.get('type') == 'loop':
                print(f"    Loop: {child.get('label', 'unnamed')} at line {child.get('line')}")
            elif child.get('type') == 'call':
                print(f"    Call: {child.get('name')} at line {child.get('line')}")
    else:
        print(f"  No children detected")

# Save outputs with debug info
analyzer.save_pretty_print_hierarchy_to_txt("debug_input_c_hierarchy.txt")
analyzer.save_hierarchy_to_json("debug_input_c_hierarchy.json")


