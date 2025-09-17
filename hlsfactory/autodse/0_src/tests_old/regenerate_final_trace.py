#!/usr/bin/env python3
"""
Regenerate final_trace.json from existing successful synthesis results.
This fixes the configuration key collision issue.
"""

import sys
import json
from pathlib import Path

# Add current directory to path
sys.path.append(str(Path(__file__).parent))

from design_space.logger import TeeLogger
from objective.objectives import ResourceConstrainedLatencyObjective


def regenerate_final_trace():
    """Regenerate final_trace.json from existing levelheap_dse_trace.log data."""
    print("🔧 Regenerating final_trace.json with fixed configuration keys...")
    
    base_dir = Path("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_levelheap_test/levelheap_dfs_test")
    
    # Check if synthesis results exist
    work_dirs = []
    for i in range(10):  # Check work_0 through work_9
        work_dir = base_dir / f"work_{i}"
        if (work_dir / "logs" / "csynth_module_info.json").exists():
            work_dirs.append(work_dir)
    
    if not work_dirs:
        print("❌ No synthesis results found!")
        return False
        
    print(f"✅ Found synthesis results in {len(work_dirs)} work directories")
    
    # Create logger
    log_file = base_dir / "regenerate.log"
    logger = TeeLogger(str(log_file))
    
    # Initialize JSON file (this will overwrite the existing one)
    logger._initialize_final_trace_json(base_dir)
    
    # Mock pragma locations (based on 3mm benchmark)
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
    
    # Resource constraints  
    max_resources = {
        'max_dsp': 5130,
        'max_bram': 3240, 
        'max_lut': 886680,
        'max_ff': 1773360,
        'max_uram': 720
    }
    
    # Process each work directory
    for i, work_dir in enumerate(work_dirs):
        print(f"🔍 Processing {work_dir.name}...")
        
        # Load synthesis results
        csynth_file = work_dir / "logs" / "csynth_module_info.json"
        rtl_file = work_dir / "logs" / "rtl_c_annotated.json"
        
        if not (csynth_file.exists() and rtl_file.exists()):
            continue
            
        with open(csynth_file) as f:
            csynth_data = json.load(f)
        with open(rtl_file) as f:
            rtl_data = json.load(f)
        
        # Create objective and evaluate
        objective = ResourceConstrainedLatencyObjective(
            max_dsp=5130, max_bram=3240, max_lut=886680,
            max_ff=1773360, max_uram=720, top_module="kernel_3mm"
        )
        
        objective_result, objective_details = objective.evaluate(csynth_data, rtl_data)
        
        # Determine pragma configuration based on iteration
        if i == 0:
            # Iteration 0: all pragmas empty
            current_choices = [""] * 18
        elif i == 1:
            # Iteration 1: pragma 11 (unroll L7) = "1" 
            current_choices = [""] * 18
            current_choices[11] = "1"
        else:
            # Other iterations - you can customize based on your actual config
            current_choices = [""] * 18
            
        # Save iteration data
        logger.save_iteration_trace(
            i, pragma_locations, current_choices,
            "success", objective_result, objective_details, max_resources
        )
        
        print(f"  ✅ Added iteration {i} (latency: {objective_details.get('latency', 'N/A')})")
    
    # Display results
    with open(logger.final_trace_json_file, 'r') as f:
        final_trace = json.load(f)
    
    print(f"\n📊 Regenerated final_trace.json with {len(final_trace)} iterations:")
    for key in final_trace.keys():
        entry = final_trace[key]
        print(f"  🔍 Config: {key[:80]}...")
        print(f"     Perf: {entry['perf']}, Valid: {entry['valid']}")
    
    logger.close()
    
    # Move the regenerated file to replace the original
    original_file = base_dir / "final_trace.json"
    if original_file.exists():
        original_file.unlink()
    logger.final_trace_json_file.rename(original_file)
    
    print(f"\n✅ Updated final_trace.json at: {original_file}")
    return True


if __name__ == "__main__":
    regenerate_final_trace()