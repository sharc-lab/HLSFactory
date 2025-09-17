#!/usr/bin/env python3
"""
Test script for the new DSE framework.
"""

import sys
from pathlib import Path

# Add current directory to path
sys.path.append(str(Path(__file__).parent))

from design_space.container import TemplateContainer
from design_space.explorer import DSEExplorer


def test_container():
    """Test template container functionality."""
    print("=== Testing TemplateContainer ===")
    
    container = TemplateContainer("../database/n256_UF4")
    template_info = container.load_template()
    
    print(f"Found {len(template_info['pragma_locations'])} pragma locations")
    print(f"Pragma types: {list(template_info['pragma_options'].keys())}")
    
    # Show some pragma details
    for i, pragma in enumerate(template_info['pragma_locations'][:3]):
        print(f"Pragma {i}: {pragma['pragma_type']} in {pragma['file']}:{pragma['line']} ({pragma['category']})")
    
    # Test variant generation
    initial_choices = container.get_initial_choices()
    print(f"Initial choices (all empty): {len(initial_choices)} pragmas")
    
    # Generate a test variant
    variant_dir = container.generate_variant(initial_choices, 0, "../0_work_dir/test_dse")
    print(f"Generated test variant in: {variant_dir}")
    
    return container


def test_explorer_analysis():
    """Test DSE explorer analysis functions without running full DSE."""
    print("\n=== Testing DSEExplorer Analysis ===")
    
    container = TemplateContainer("../database/n256_UF4")
    explorer = DSEExplorer(container, target_ii=1)
    
    # Test with existing analysis results
    analysis_dir = Path("../0_work_dir/0_concrete_design/logs")
    
    if (analysis_dir / "csynth_module_info.json").exists():
        import json
        
        with open(analysis_dir / "csynth_module_info.json") as f:
            csynth_data = json.load(f)
        
        with open(analysis_dir / "rtl_c_annotated.json") as f:
            rtl_data = json.load(f)
        
        print("Testing critical path analysis...")
        bottleneck = explorer.find_critical_path_and_bottleneck(csynth_data, rtl_data, 1)
        print(f"Identified bottleneck: {bottleneck}")
        
        print("Testing pragma mapping...")
        affecting_indices = explorer.find_affecting_pragmas(bottleneck, rtl_data)
        print(f"Found {len(affecting_indices)} affecting pragma indices: {affecting_indices}")
        
        print("Testing pragma sampling...")
        new_choices = explorer.sample_next_pragmas(affecting_indices)
        print(f"Sampled new choices: {len([c for c in new_choices if c != ''])} non-empty pragmas")
        
    else:
        print("No existing analysis results found, skipping analysis tests")


def test_mini_dse():
    """Test a mini DSE run with 2 iterations."""
    print("\n=== Testing Mini DSE Run ===")
    
    container = TemplateContainer("../database/n256_UF4")
    explorer = DSEExplorer(container, target_ii=1)
    
    # Run just 2 iterations
    output_dir = "../0_work_dir/test_dse_mini"
    try:
        results = explorer.run_exploration(max_iterations=2, output_dir=output_dir, timeout_min=5)
        print(f"Mini DSE completed with {len(results)} iterations")
        
        for result in results:
            print(f"  Iteration {result['iter']}: {result['status']}")
            
    except Exception as e:
        print(f"Mini DSE failed: {e}")
        import traceback
        traceback.print_exc()


if __name__ == "__main__":
    # Test container
    container = test_container()
    
    # Test explorer analysis
    test_explorer_analysis()
    
    # Ask user if they want to run mini DSE (requires HLS tools)
    print("\n" + "="*50)
    response = input("Run mini DSE test (requires HLS tools)? [y/N]: ")
    if response.lower() == 'y':
        test_mini_dse()
    else:
        print("Skipping mini DSE test")
    
    print("\n✅ DSE framework tests completed!")