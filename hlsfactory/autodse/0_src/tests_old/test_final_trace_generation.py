#!/usr/bin/env python3
"""
Test script to generate final_trace.json from existing successful synthesis results.
This demonstrates that the JSON generation logic works correctly.
"""

import sys
import json
from pathlib import Path

# Add current directory to path
sys.path.append(str(Path(__file__).parent))

from design_space.logger import TeeLogger


def test_json_generation():
    """Test final_trace.json generation using existing synthesis results."""
    print("🧪 Testing final_trace.json generation...")
    
    # Use existing successful synthesis results
    base_dir = Path("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_levelheap_test/levelheap_dfs_test")
    test_output_dir = base_dir / "test_json_generation"
    test_output_dir.mkdir(exist_ok=True)
    
    # Create logger
    log_file = test_output_dir / "test.log"
    logger = TeeLogger(str(log_file))
    
    # Mock pragma locations (typical for 3mm benchmark)
    pragma_locations = [
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 23, "region": "L0"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 25, "region": "L0"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 29, "region": "L3"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 31, "region": "L3"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 36, "region": "L6"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 38, "region": "L6"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 47, "region": "L1"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 49, "region": "L1"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 53, "region": "L4"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 55, "region": "L4"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 60, "region": "L7"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 62, "region": "L7"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 71, "region": "L2"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 73, "region": "L2"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 77, "region": "L5"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 79, "region": "L5"},
        {"pragma_type": "pipeline", "file": "3mm.cpp", "line": 84, "region": "L8"},
        {"pragma_type": "unroll", "file": "3mm.cpp", "line": 86, "region": "L8"}
    ]
    
    # Initialize the final trace JSON file manually
    logger._initialize_final_trace_json(test_output_dir)
    
    print(f"✅ JSON file initialized at: {logger.final_trace_json_file}")
    
    # Test multiple iterations with different pragma configurations
    test_cases = [
        {
            "iteration": 0,
            "choices": ["", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""],
            "status": "success",
            "latency": 1411957,
            "dsp": 368, "bram": 100, "lut": 101151, "ff": 64162, "uram": 4,
            "valid": False  # Exceeds constraints
        },
        {
            "iteration": 1, 
            "choices": ["", "", "", "", "", "", "", "", "", "", "", "1", "", "", "", "", "", ""],
            "status": "success", 
            "latency": 1411957,
            "dsp": 368, "bram": 100, "lut": 101151, "ff": 64162, "uram": 4,
            "valid": False  # Still exceeds constraints
        },
        {
            "iteration": 2,
            "choices": ["", "", "", "", "", "", "", "", "flatten", "", "", "2", "", "", "", "", "", ""],
            "status": "success",
            "latency": 700000,
            "dsp": 2000, "bram": 1500, "lut": 500000, "ff": 800000, "uram": 200,
            "valid": True  # Within constraints
        }
    ]
    
    # Resource constraints for ResourceConstrainedLatencyObjective  
    max_resources = {
        'max_dsp': 5130,
        'max_bram': 3240, 
        'max_lut': 886680,
        'max_ff': 1773360,
        'max_uram': 720
    }
    
    for test_case in test_cases:
        # Create mock objective details
        obj_details = {
            'latency': test_case['latency'],
            'dsp_used': test_case['dsp'],
            'bram_used': test_case['bram'],
            'lut_used': test_case['lut'], 
            'ff_used': test_case['ff'],
            'uram_used': test_case['uram'],
            'resource_violations': [] if test_case['valid'] else ["DSP: exceeds limit"]
        }
        
        logger.save_iteration_trace(
            test_case['iteration'], 
            pragma_locations, 
            test_case['choices'],
            test_case['status'],
            None,  # obj_result 
            obj_details,
            max_resources
        )
        
        print(f"✅ Added iteration {test_case['iteration']} to JSON")
    
    # Read and display the generated JSON
    with open(logger.final_trace_json_file, 'r') as f:
        final_trace = json.load(f)
    
    print(f"\n📊 Generated final_trace.json with {len(final_trace)} iterations:")
    for key in list(final_trace.keys())[:2]:  # Show first 2 entries
        print(f"\n🔍 {key}:")
        entry = final_trace[key] 
        print(f"  perf: {entry['perf']}")
        print(f"  valid: {entry['valid']}")
        print(f"  resources: DSP={entry['res_util']['total-DSP']}, BRAM={entry['res_util']['total-BRAM']}")
        print(f"  utilization: DSP={entry['res_util']['util-DSP']:.3f}, BRAM={entry['res_util']['util-BRAM']:.3f}")
    
    logger.close()
    print(f"\n🎉 Test completed! Final trace JSON saved at:")
    print(f"   {logger.final_trace_json_file}")
    
    return True


if __name__ == "__main__":
    try:
        test_json_generation()
        print("\n✅ JSON generation test PASSED!")
    except Exception as e:
        print(f"\n❌ JSON generation test FAILED: {e}")
        import traceback
        traceback.print_exc()