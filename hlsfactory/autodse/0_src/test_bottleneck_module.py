#!/usr/bin/env python3
"""
Test script for bottleneck module detection functionality.
Tests the objective function bottleneck detection logic from lines 595-610 in explorer.py.
"""

import json
import sys
from pathlib import Path

# Add the current directory to sys.path to import modules
sys.path.append(str(Path(__file__).parent))
from objective.objectives import TargetIIObjective, ResourceConstrainedLatencyObjective
from design_space.explorer import DSEExplorer
from design_space.container import TemplateContainer


def load_test_data():
    """Load test data from the specified log folder."""
    logs_dir = Path("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_vitishlssyn_3mm/dse_resource_latency/work_0/logs")
    csynth_file = logs_dir / "csynth_module_info.json"
    rtl_file = logs_dir / "rtl_c_annotated.json"
    
    if not (csynth_file.exists() and rtl_file.exists()):
        print("❌ Required test data files not found:")
        print(f"  Expected: {csynth_file}")
        print(f"  Expected: {rtl_file}")
        return None, None
    
    try:
        with open(csynth_file) as f:
            csynth_data = json.load(f)
        with open(rtl_file) as f:
            rtl_data = json.load(f)
        
        print(f"✅ Successfully loaded test data from {logs_dir}")
        return csynth_data, rtl_data
    except Exception as e:
        print(f"❌ Error loading test data: {e}")
        return None, None


def print_affecting_pragmas(bottleneck_module, rtl_data, csynth_data):
    """Find and print pragmas affecting the bottleneck module using hierarchical ordering."""
    template_dir = "/home/cy/github/DSE-HLS-FFT/database/vitishlssyn/3mm"
    container = TemplateContainer(template_dir)
    explorer = DSEExplorer(container)
    
    # Add debug logger to see what hierarchy file is being loaded
    class DebugLogger:
        def print(self, msg):
            print(msg)
    explorer.logger = DebugLogger()
    
    affecting_indices = explorer.find_affecting_pragmas(bottleneck_module, rtl_data)
    
    print("\n=== ORIGINAL PRAGMA ORDERING (Simple Priority) ===")
    ordered_pragmas_simple = explorer.order_pragma_lists(affecting_indices)
    print("Ordered pragmas for current bottleneck (simple):")
    for i, pragma_dict in enumerate(ordered_pragmas_simple):
        pragma_idx = pragma_dict['index']
        if pragma_idx < len(explorer.pragma_locations):
            pragma_info = explorer.pragma_locations[pragma_idx]
            pragma_type = pragma_info['pragma_type']
            file = pragma_info['file']
            line = pragma_info['line']
            region = pragma_info.get('region', 'unknown')
            current_choice = explorer.current_choices[pragma_idx] if pragma_idx < len(explorer.current_choices) else "N/A"
            
            print(f"  {i+1}. [{pragma_idx}] {pragma_type}@{file}:{line}, {region} -> '{current_choice}'")
    
    print("\n=== NEW HIERARCHICAL PRAGMA ORDERING (Latency-Based) ===")
    logs_dir = Path("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_vitishlssyn_3mm/dse_resource_latency/work_0/logs")
    work_dir = str(logs_dir.parent)  # work_0 directory
    hierarchy_file = logs_dir / "input_c_hierarchy.json"
    ordered_pragmas_hierarchical = explorer.order_pragma_lists_hierarchical(affecting_indices, csynth_data, rtl_data, work_dir, str(hierarchy_file))
    print("Ordered pragmas for current bottleneck (hierarchical):")
    for i, pragma_dict in enumerate(ordered_pragmas_hierarchical):
        pragma_idx = pragma_dict['index']
        if pragma_idx < len(explorer.pragma_locations):
            pragma_info = explorer.pragma_locations[pragma_idx]
            pragma_type = pragma_info['pragma_type']
            file = pragma_info['file']
            line = pragma_info['line']
            region = pragma_info.get('region', 'unknown')
            current_choice = explorer.current_choices[pragma_idx] if pragma_idx < len(explorer.current_choices) else "N/A"
            
            print(f"  {i+1}. [{pragma_idx}] {pragma_type}@{file}:{line}, {region} -> '{current_choice}'")


def test_target_ii_objective():
    """Test TargetIIObjective bottleneck detection."""
    print("\n=== Testing TargetIIObjective (target_ii_32) ===")
    
    # Load test data
    csynth_data, rtl_data = load_test_data()
    if not csynth_data or not rtl_data:
        print("❌ Cannot test without data files")
        return
    
    # Get the actual top module name from the data
    top_module_name = csynth_data.get("top_module", {}).get("ModuleName", "kernel_3mm")
    print(f"🔍 Using top module: {top_module_name}")
    
    # Create objective with the correct top module name
    objective = TargetIIObjective(target_ii=32, top_module=top_module_name)
    
    # Evaluate objective and extract bottleneck module
    objective_result, objective_details = objective.evaluate(csynth_data, rtl_data)
    bottleneck_module = objective_details.get("bottleneck_module", "FFT_TOP")
    
    # Display results
    print(f"📊 Objective Result: {objective_result.value}")
    print(f"🎯 Bottleneck Module: {bottleneck_module}")
    print(f"📋 Full Details: {objective_details}")
    
    # Show top module metrics if available
    if "metrics" in csynth_data and top_module_name in csynth_data["metrics"]:
        top_metrics = csynth_data["metrics"][top_module_name]
        current_ii = top_metrics.get("PipelineInitiationInterval", "N/A")
        print(f"🔍 Top Module II: {current_ii}")
        
        # Check if target is met
        target_met = current_ii <= 32 if isinstance(current_ii, (int, float)) else False
        print(f"✅ Target Met (II ≤ 32): {target_met}")
    
    # Find and print affecting pragmas
    if bottleneck_module:
        print_affecting_pragmas(bottleneck_module, rtl_data, csynth_data)

    return bottleneck_module, objective_result, objective_details


def test_resource_constrained_latency_objective():
    """Test ResourceConstrainedLatencyObjective bottleneck detection."""
    print("\n=== Testing ResourceConstrainedLatencyObjective (resource_constrained_latency) ===")
    
    # Load test data
    csynth_data, rtl_data = load_test_data()
    if not csynth_data or not rtl_data:
        print("❌ Cannot test without data files")
        return
    
    # Get the actual top module name from the data
    top_module_name = csynth_data.get("top_module", {}).get("ModuleName", "kernel_3mm")
    print(f"🔍 Using top module: {top_module_name}")
    
    # Create objective with the correct top module name
    objective = ResourceConstrainedLatencyObjective(
        max_dsp=5130, 
        max_bram=3240, 
        max_lut=886680,
        max_ff=1773360,
        max_uram=720,
        top_module=top_module_name
    )
    
    # Evaluate objective and extract bottleneck module
    objective_result, objective_details = objective.evaluate(csynth_data, rtl_data)
    bottleneck_module = objective_details.get("bottleneck_module", "FFT_TOP")
    
    # Display results
    print(f"📊 Objective Result: {objective_result.value}")
    print(f"🎯 Bottleneck Module: {bottleneck_module}")
    print(f"📋 Full Details: {objective_details}")
    
    # Show resource usage if available
    if "metrics" in csynth_data and top_module_name in csynth_data["metrics"]:
        top_metrics = csynth_data["metrics"][top_module_name]
        
        resources = {
            "DSP": top_metrics.get("DSP", 0),
            "BRAM_18K": top_metrics.get("BRAM_18K", 0),
            "LUT": top_metrics.get("LUT", 0),
            "FF": top_metrics.get("FF", 0),
            "URAM": top_metrics.get("URAM", 0)
        }
        
        latency = top_metrics.get("PipelineLatency", "N/A")
        
        print(f"🔍 Resource Usage:")
        print(f"  DSP: {resources['DSP']} / {5130} ({resources['DSP']/5130*100:.1f}%)")
        print(f"  BRAM: {resources['BRAM_18K']} / {3240} ({resources['BRAM_18K']/3240*100:.1f}%)")
        print(f"  LUT: {resources['LUT']} / {886680} ({resources['LUT']/886680*100:.1f}%)")
        print(f"  FF: {resources['FF']} / {1773360} ({resources['FF']/1773360*100:.1f}%)")
        print(f"  URAM: {resources['URAM']} / {720} ({resources['URAM']/720*100:.1f}%)")
        print(f"  Latency: {latency}")
        
        # Check resource violations
        violations = objective_details.get("resource_violations", [])
        if violations:
            print(f"⚠️  Resource Violations: {violations}")
        else:
            print(f"✅ All resource constraints satisfied")
    
    # Find and print affecting pragmas
    if bottleneck_module:
        print_affecting_pragmas(bottleneck_module, rtl_data, csynth_data)

    return bottleneck_module, objective_result, objective_details


def display_data_summary(csynth_data, rtl_data):
    """Display summary of loaded test data."""
    print("\n=== Test Data Summary ===")
    
    # Show top-level structure
    print(f"📁 csynth_data keys: {list(csynth_data.keys())}")
    print(f"📁 rtl_data keys: {list(rtl_data.keys())}")
    
    # Show available modules in metrics
    if "metrics" in csynth_data:
        modules = list(csynth_data["metrics"].keys())
        print(f"🏗️  Available modules ({len(modules)}): {modules}")
        
        # Show top module info  
        top_module_name = csynth_data.get("top_module", {}).get("ModuleName", "Unknown")
        if top_module_name in csynth_data["metrics"]:
            top_metrics = csynth_data["metrics"][top_module_name]
            print(f"🔝 {top_module_name} metrics: {list(top_metrics.keys())}")
    
    # Show RTL hierarchy structure
    if "rtl_hierarchy" in rtl_data:
        top_module = rtl_data["rtl_hierarchy"].get("ModuleName", "Unknown")
        children_count = len(rtl_data["rtl_hierarchy"].get("Children", []))
        print(f"🌳 RTL Hierarchy - Top: {top_module}, Children: {children_count}")
    
    if "modules" in rtl_data:
        rtl_modules = list(rtl_data["modules"].keys())
        print(f"🏗️  RTL modules ({len(rtl_modules)}): {rtl_modules}")


def run_comparative_analysis():
    """Compare bottleneck detection between both objectives."""
    print("\n=== Comparative Analysis ===")
    
    # Test both objectives
    target_ii_result = test_target_ii_objective()
    resource_result = test_resource_constrained_latency_objective()
    
    if target_ii_result[0] and resource_result[0]:
        target_ii_bottleneck, target_ii_obj_result, target_ii_details = target_ii_result
        resource_bottleneck, resource_obj_result, resource_details = resource_result
        
        print(f"\n🔀 Bottleneck Comparison:")
        print(f"  TargetII (II≤32): {target_ii_bottleneck}")
        print(f"  ResourceLatency: {resource_bottleneck}")
        
        if target_ii_bottleneck == resource_bottleneck:
            print(f"✅ Both objectives identified the same bottleneck module")
        else:
            print(f"⚠️  Different bottleneck modules identified - this is expected for different objectives")
        
        print(f"\n📊 Result Comparison:")
        print(f"  TargetII Result: {target_ii_obj_result.value}")
        print(f"  ResourceLatency Result: {resource_obj_result.value}")


def main():
    """Main test function."""
    print("🧪 Testing Bottleneck Module Detection from Explorer.py (lines 595-610)")
    print("=" * 80)
    
    # Load and summarize test data
    csynth_data, rtl_data = load_test_data()
    if not csynth_data or not rtl_data:
        print("❌ Cannot proceed without test data")
        return
    
    display_data_summary(csynth_data, rtl_data)
    
    # Run comparative analysis
    run_comparative_analysis()
    
    print("\n" + "=" * 80)
    print("🧪 Bottleneck module detection tests completed!")
    print("\nThis test simulates the explorer.py logic (lines 595-610):")
    print("1. Load synthesis results from logs directory")
    print("2. Evaluate objective function with csynth_data and rtl_data")  
    print("3. Extract bottleneck_module from objective_details")
    print("4. Compare results between different objective functions")


if __name__ == "__main__":
    main()