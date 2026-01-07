#!/usr/bin/env python3
"""
LevelHeap DFS Test Script
This script demonstrates the new LevelHeap DFS exploration method with ResourceConstrainedLatencyObjective.

Usage:
    python test_dse_LevelHeap_DFS.py [TEMPLATE_DIR] [BASE_OUTPUT_DIR]

Arguments:
    TEMPLATE_DIR     : Path to benchmark template directory (optional)
    BASE_OUTPUT_DIR  : Path to output directory (optional)

If arguments are not provided, defaults are used.
"""

import sys
import os
import shutil
import argparse
from pathlib import Path
from datetime import datetime

# Add current directory to path
sys.path.append(str(Path(__file__).parent))

from design_space.container import TemplateContainer
from design_space.explorer import DSEExplorer
from design_space.logger import TeeLogger
from objective.objectives import ResourceConstrainedLatencyObjective

def int_or_none(v):
    return None if v.lower() == 'none' else int(v)
# ===================================================================
# ARGUMENT PARSING
# ===================================================================

def parse_arguments():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(
        description='Run LevelHeap DFS exploration for HLS design space exploration',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
    python test_dse_LevelHeap_DFS.py
    python test_dse_LevelHeap_DFS.py /path/to/template /path/to/output
    python test_dse_LevelHeap_DFS.py database/vitishlssyn/3mm 0_work_dir/dse_test/3mm
        """
    )
    
    parser.add_argument(
        'template_dir', 
        nargs='?',
        default="/home/cy/github/DSE-HLS-FFT/database/vitishlssyn/syr2k",
        help='Path to benchmark template directory (default: %(default)s)'
    )
    
    parser.add_argument(
        'output_dir',
        nargs='?', 
        default="/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_levelheap_test/syr2k",
        help='Path to base output directory (default: %(default)s)'
    )
    
    parser.add_argument(
        '--max-iterations', '-i',
        type=int_or_none,
        default=3,
        help='Maximum number of iterations (use None for time-based exploration)'
    )
    
    parser.add_argument(
        '--hours', '-t',
        type=float,
        default=24,
        help='Exploration time in hours for time-based exploration (default: %(default)s)'
    )
    
    parser.add_argument(
        '--timeout',
        type=int,
        default=2,
        help='Synthesis timeout in minutes (default: %(default)s)'
    )
    
    parser.add_argument(
        '--resume',
        # action='store_true',
        default=False,
        help='Resume from checkpoint if available'
    )
    
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        default=True,
        help='Enable verbose logging (default: True)'
    )
    
    return parser.parse_args()

# Parse command-line arguments
args = parse_arguments()

# ===================================================================
# CONFIGURATION PARAMETERS
# ===================================================================

# Path Configuration (from command-line arguments)
TEMPLATE_DIR = args.template_dir
BASE_OUTPUT_DIR = args.output_dir

# DSE Parameters (from command-line arguments)
MAX_ITERATIONS = args.max_iterations
EXPLORATION_HOURS = args.hours
TIMEOUT_MINUTES = args.timeout
RESUME_FROM_CHECKPOINT = args.resume
VERBOSE_LOGGING = args.verbose

# Print configuration
print(f"🔧 Configuration:")
print(f"   Template Dir: {TEMPLATE_DIR}")
print(f"   Output Dir: {BASE_OUTPUT_DIR}")
if MAX_ITERATIONS is not None:
    print(f"   Max Iterations: {MAX_ITERATIONS}")
else:
    print(f"   Exploration Time: {EXPLORATION_HOURS} hours")
print(f"   Synthesis Timeout: {TIMEOUT_MINUTES} minutes")
print(f"   Resume from Checkpoint: {RESUME_FROM_CHECKPOINT}")
print(f"   Verbose Logging: {VERBOSE_LOGGING}")
print()

def extract_top_module_from_tcl(template_dir):
    """
    Extract top module name from project.tcl file or other template files.
    
    Args:
        template_dir: Path to template directory
        
    Returns:
        Top module name or None if not found
    """
    import re
    
    # First try to extract from project.tcl
    tcl_file = Path(template_dir) / "project.tcl"
    if tcl_file.exists():
        try:
            with open(tcl_file, 'r') as f:
                for line in f:
                    line = line.strip()
                    # Look for set_top command
                    if line.startswith('set_top'):
                        # Extract module name from quotes
                        # Format: set_top "module_name"
                        match = re.search(r'set_top\s+["\']([^"\']+)["\']', line)
                        if match:
                            return match.group(1)
                        # Also handle format without quotes: set_top module_name
                        match = re.search(r'set_top\s+(\S+)', line)
                        if match:
                            return match.group(1)
        except Exception as e:
            print(f"Warning: Could not parse project.tcl: {e}")
    
    # If project.tcl doesn't exist or doesn't contain set_top, try to infer from directory name or files
    template_path = Path(template_dir)
    template_name = template_path.name
    
    # Check for common patterns in template directory names
    if 'fft' in template_name.lower():
        return 'FFT_TOP'
    elif '3mm' in template_name.lower():
        return 'kernel_3mm'
    elif template_name.startswith('kernel_'):
        return template_name
    
    # Look for .cpp files and try to infer top function name
    cpp_files = list(template_path.glob('*.cpp'))
    for cpp_file in cpp_files:
        if 'testbench' not in cpp_file.name.lower():
            # Use the cpp file name as basis for top module
            base_name = cpp_file.stem
            if base_name.lower() in ['fft', 'fft_top']:
                return 'FFT_TOP'
            else:
                # Assume kernel_ prefix for other designs
                return f'kernel_{base_name}'
    
    return None

def run_levelheap_dfs_test():
    """Run LevelHeap DFS test with ResourceConstrainedLatencyObjective."""
    output_dir = f"{BASE_OUTPUT_DIR}"
    output_path = Path(output_dir)
    
    # Handle directory cleanup if not resuming from checkpoint
    if not RESUME_FROM_CHECKPOINT:
        print(f"🧹 Clearing output directory: {output_dir}")
        if output_path.exists():
            shutil.rmtree(output_path)
        output_path.mkdir(parents=True, exist_ok=True)
    else:
        output_path.mkdir(parents=True, exist_ok=True)
    
    # Initialize logger
    logger = TeeLogger(output_path / "explore.log", console_output=True)
    
    try:
        logger.print("🚀 LevelHeap DFS Test")
        logger.print("=" * 70)
        logger.print(f"Objective: Minimize latency with resource constraints")
        logger.print(f"Template: {TEMPLATE_DIR}")
        logger.print(f"Output: {output_dir}")
        if MAX_ITERATIONS is not None:
            logger.print(f"Max iterations: {MAX_ITERATIONS}")
        else:
            logger.print(f"Exploration time: {EXPLORATION_HOURS} hours")
        logger.print(f"Timeout: {TIMEOUT_MINUTES} minutes")
        logger.print("=" * 70)
        
        # Extract top module name from template
        container = TemplateContainer(TEMPLATE_DIR)
        
        # Try to get top module from template info first
        top_module_name = None
        
        # If not found, try to extract from project.tcl file
        if top_module_name is None:
            top_module_name = extract_top_module_from_tcl(TEMPLATE_DIR)
        
        # Final fallback if still not found
        if top_module_name is None:
            top_module_name = "unknown_top"
            logger.print("⚠️ Could not detect top module, using fallback name")
        
        logger.print(f"🔍 Using top module: {top_module_name}")
        
        # Create objective with the specified configuration
        objective = ResourceConstrainedLatencyObjective(
            max_dsp=5472, 
            max_bram=3456, 
            max_lut=945792,
            max_ff=1891584,
            max_uram=768,
            top_module=top_module_name
        )

        # Create explorer
        explorer = DSEExplorer(
            container=container,
            objective_fn=objective
        )
        
        # Set logger
        explorer.set_logger(logger)
        
        logger.print(f"✅ Template loaded: {len(explorer.pragma_locations)} pragma locations")
        logger.print(f"✅ Explorer initialized with objective: {objective.get_target_description()}")
        logger.print()
        
        # Check checkpoint status
        if RESUME_FROM_CHECKPOINT:
            checkpoint_file = output_path / "pragma_state.pkl"
            if checkpoint_file.exists():
                logger.print(f"📂 Resuming from checkpoint: {checkpoint_file}")
            else:
                logger.print(f"📁 No checkpoint found, starting fresh")
        else:
            logger.print(f"🆕 Starting fresh exploration")
        
        logger.print(f"\n🔄 Starting LevelHeap DFS exploration...")
        
        # Run LevelHeap DFS with time-based or iteration-based exploration
        if MAX_ITERATIONS is not None:
            # Use iteration-based exploration
            results = explorer.run_exploration_LevelHeap_DFS(
                max_iterations=MAX_ITERATIONS,
                output_dir=output_dir,
                timeout_min=TIMEOUT_MINUTES
            )
        else:
            # Use time-based exploration
            results = explorer.run_exploration_LevelHeap_DFS(
                max_iterations=float('inf'),  # No iteration limit
                output_dir=output_dir,
                timeout_min=TIMEOUT_MINUTES,
                max_exploration_hours=EXPLORATION_HOURS
            )
        
        logger.print(f"\n🎉 LevelHeap DFS completed: {len(results)} iterations!")
        
        # Analyze results
        analyze_results(results, logger)
        
        return True, len(results)
        
    except KeyboardInterrupt:
        logger.print("\n⚠️ LevelHeap DFS interrupted by user")
        return False, 0
        
    except Exception as e:
        logger.print(f"\n❌ LevelHeap DFS failed: {e}")
        if VERBOSE_LOGGING:
            import traceback
            logger.print(traceback.format_exc())
        return False, 0
        
    finally:
        logger.close()


def analyze_results(results, logger):
    """Analyze and display results for LevelHeap DFS exploration."""
    logger.print("\n" + "=" * 50)
    logger.print("📊 LEVELHEAP DFS RESULTS ANALYSIS")
    logger.print("=" * 50)
    
    successful_runs = 0
    failed_runs = 0
    infeasible_runs = 0
    best_score = float('inf')
    best_iteration = None
    best_details = None
    
    logger.print(f"\n📋 Iteration Summary:")
    for result in results:
        status = result['status']
        iteration = result['iter']
        
        if status == "success":
            successful_runs += 1
            status_icon = "✅"
            
            # Extract objective evaluation
            if 'metrics' in result and 'objective_result' in result['metrics']:
                obj_result = result['metrics']['objective_result']
                obj_details = result['metrics'].get('objective_details', {})
                
                # Check if design is infeasible due to resource constraints
                if obj_result == "infeasible":
                    infeasible_runs += 1
                    status_icon = "❌💾"  # Resource violation
                    violations = obj_details.get('resource_violations', [])
                    logger.print(f"  {status_icon} Iteration {iteration:2d}: INFEASIBLE - Resource violations: {violations}")
                else:
                    # Extract latency score
                    raw_score = obj_details.get('latency', float('inf'))
                    try:
                        score = float(raw_score)
                    except (ValueError, TypeError):
                        score = float('inf')
                    
                    if score < best_score:
                        best_score = score
                        best_iteration = iteration
                        best_details = obj_details
                    
                    logger.print(f"  {status_icon} Iteration {iteration:2d}: {obj_result} (latency: {score})")
                    
                    # Show key metrics
                    if 'dsp_used' in obj_details:
                        logger.print(f"     Resources: DSP={obj_details['dsp_used']}, BRAM={obj_details['bram_used']}, LUT={obj_details['lut_used']}")
                    if 'bottleneck_module' in obj_details:
                        logger.print(f"     Bottleneck: {obj_details['bottleneck_module']}")
            else:
                logger.print(f"  {status_icon} Iteration {iteration:2d}: SUCCESS (no objective details)")
        else:
            failed_runs += 1
            status_icon = "❌"
            logger.print(f"  {status_icon} Iteration {iteration:2d}: {status.upper()}")
    
    # Summary statistics
    total_runs = successful_runs + failed_runs
    success_rate = (successful_runs / total_runs * 100) if total_runs > 0 else 0
    feasible_runs = successful_runs - infeasible_runs
    feasible_rate = (feasible_runs / total_runs * 100) if total_runs > 0 else 0
    
    logger.print(f"\n📈 Summary:")
    logger.print(f"  Total iterations: {total_runs}")
    logger.print(f"  Successful synthesis: {successful_runs}")
    logger.print(f"  Failed synthesis: {failed_runs}")
    logger.print(f"  Feasible designs: {feasible_runs}")
    logger.print(f"  Infeasible designs: {infeasible_runs}")
    logger.print(f"  Success rate: {success_rate:.1f}%")
    logger.print(f"  Feasible rate: {feasible_rate:.1f}%")
    
    # Best result
    if best_iteration is not None:
        logger.print(f"\n🏆 Best Feasible Result (Iteration {best_iteration}):")
        logger.print(f"  Latency: {best_score}")
        if best_details:
            logger.print(f"  Resources:")
            for resource in ['dsp_used', 'bram_used', 'lut_used', 'ff_used', 'uram_used']:
                if resource in best_details:
                    logger.print(f"    {resource.upper()}: {best_details[resource]}")
            if 'bottleneck_module' in best_details:
                logger.print(f"  Bottleneck Module: {best_details['bottleneck_module']}")
    else:
        logger.print(f"\n⚠️ No feasible designs found within resource constraints")
    
    # LevelHeap specific analysis
    logger.print(f"\n🔍 LevelHeap DFS Analysis:")
    logger.print(f"  This exploration used the LevelHeap DFS algorithm which:")
    logger.print(f"  - Explores pragma configurations level by level")
    logger.print(f"  - Uses DFS to go deep into promising branches first")
    logger.print(f"  - Focuses on bottleneck modules for pragma selection")
    logger.print(f"  - Marks designs as 'failure' when resource constraints are exceeded")


def main():
    """Run the LevelHeap DFS test."""
    print("🚀 LevelHeap DFS Test Suite")
    print("=" * 80)
    print("This test demonstrates the new LevelHeap DFS exploration method")
    print("with ResourceConstrainedLatencyObjective that:")
    print("  • Minimizes latency while respecting resource constraints")
    print("  • Labels designs as 'failure' when resources are exceeded")
    print("  • Uses bottleneck analysis to guide pragma exploration")
    print("  • Orders pragmas by importance for each bottleneck module")
    print()
    
    # Confirm template directory exists
    if not Path(TEMPLATE_DIR).exists():
        print(f"❌ Template directory not found: {TEMPLATE_DIR}")
        print("Please update TEMPLATE_DIR in the script to point to a valid template.")
        return
    
    print(f"📁 Template directory: {TEMPLATE_DIR}")
    print(f"📂 Output directory: {BASE_OUTPUT_DIR}")
    if MAX_ITERATIONS is not None:
        print(f"🔧 Max iterations: {MAX_ITERATIONS}")
    else:
        print(f"🕐 Exploration time: {EXPLORATION_HOURS} hours")
    print(f"⏱️ Timeout per synthesis: {TIMEOUT_MINUTES} minutes")
    print()
    
    # Ask user for confirmation (auto-accept if running in non-interactive mode)
    # try:
    #     if input("Proceed with LevelHeap DFS test? (y/N): ").strip().lower() != 'y':
    #         print("Test cancelled.")
    #         return
    # except EOFError:
    #     # Running in non-interactive mode, proceed automatically
    #     print("Running in non-interactive mode, proceeding automatically...")
    #     pass
    
    print(f"\n🔄 Running LevelHeap DFS test...\n")
    
    # Run the test
    success, iterations = run_levelheap_dfs_test()
    
    # Final summary
    print(f"\n{'='*80}")
    print("📊 FINAL SUMMARY")
    print(f"{'='*80}")
    
    if success:
        print(f"✅ LevelHeap DFS test completed successfully")
        print(f"   Iterations completed: {iterations}")
        print(f"   Output directory: {BASE_OUTPUT_DIR}")
        print(f"   Log file: {BASE_OUTPUT_DIR}/explore.log")
    else:
        print(f"❌ LevelHeap DFS test failed")
    
    print("\n🔍 Key Features Demonstrated:")
    print("  • LevelHeap DFS algorithm implementation")
    print("  • Resource-constrained latency optimization") 
    print("  • Automatic bottleneck module identification")
    print("  • Pragma importance ordering for exploration")
    print("  • Design feasibility checking with resource constraints")


if __name__ == "__main__":
    main()