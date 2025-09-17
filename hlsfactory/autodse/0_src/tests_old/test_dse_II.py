#!/usr/bin/env python3
"""
Parameterized DSE Test Script
Configure the parameters below before running the test.
"""

import sys
import os
import shutil
from pathlib import Path
from datetime import datetime

# Add current directory to path
sys.path.append(str(Path(__file__).parent))

from design_space.container import TemplateContainer
from design_space.explorer import DSEExplorer
from design_space.logger import TeeLogger

# ===================================================================
# USER CONFIGURATION PARAMETERS
# Modify these values before running the DSE test
# ===================================================================

# DSE Parameters
TARGET_II = 32                          # Target initiation interval
MAX_ITERATIONS = 2                      # Number of DSE iterations to run
TIMEOUT_MINUTES = 10                    # HLS synthesis timeout per iteration

# Path Configuration
TEMPLATE_DIR = "/home/cy/github/DSE-HLS-FFT/database/n256_UF4"   # Template directory path
OUTPUT_DIR = "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run"    # Output directory for results
TCL_FILENAME = "FFT_project.tcl"        # TCL file name in template

# Advanced Options
RESUME_FROM_CHECKPOINT = False          # Resume from existing checkpoint if available 
VERBOSE_LOGGING = True                 # Enable detailed logging
# ===================================================================
# END USER CONFIGURATION
# ===================================================================


# Global logger instance (will be initialized in main)
logger = None


def main():
    """Run parameterized DSE test using the configured parameters."""
    global logger
    
    # Initialize output path early
    output_path = Path(OUTPUT_DIR)
    
    # Handle directory cleanup if RESUME_FROM_CHECKPOINT=FALSE (before initializing logger)
    if not RESUME_FROM_CHECKPOINT:
        print(f"🧹 RESUME_FROM_CHECKPOINT=FALSE - Clearing output directory")
        
        # Remove all contents in the output directory
        if output_path.exists():
            for item in output_path.iterdir():
                if item.is_file():
                    item.unlink()
                    print(f"   Removed file: {item.name}")
                elif item.is_dir():
                    shutil.rmtree(item)
                    print(f"   Removed directory: {item.name}")
        
        # Ensure output directory exists (empty)
        output_path.mkdir(parents=True, exist_ok=True)
        print(f"   ✅ Output directory cleared and ready: {OUTPUT_DIR}")
        print()
    
    # Initialize logger to write to explore.log
    logger = TeeLogger(output_path / "explore.log", console_output=True)
    
    logger.print(f"🚀 Parameterized DSE Test")
    logger.print("=" * 60)
    
    # Display configuration
    logger.print("Configuration:")
    logger.print(f"  Target II: {TARGET_II}")
    logger.print(f"  Max iterations: {MAX_ITERATIONS}")
    logger.print(f"  Template directory: {TEMPLATE_DIR}")
    logger.print(f"  Output directory: {OUTPUT_DIR}")
    logger.print(f"  TCL filename: {TCL_FILENAME}")
    logger.print(f"  HLS timeout: {TIMEOUT_MINUTES} minutes")
    logger.print(f"  Resume from checkpoint: {RESUME_FROM_CHECKPOINT}")
    logger.print("=" * 60)
    
    try:
        # Create container and explorer with parameterized values
        container = TemplateContainer(TEMPLATE_DIR)
        explorer = DSEExplorer(container, target_ii=TARGET_II)
        
        # Set logger for explorer (which will propagate to container)
        explorer.set_logger(logger)
        
        logger.print(f"✅ Template loaded: {len(explorer.pragma_locations)} pragma locations")
        logger.print(f"✅ Explorer initialized with target II = {TARGET_II}")
        logger.print()
        
        # Check if we should resume from checkpoint
        if RESUME_FROM_CHECKPOINT:
            checkpoint_file = output_path / "pragma_state.pkl"
            if checkpoint_file.exists():
                logger.print(f"📂 Found existing checkpoint: {checkpoint_file}")
                logger.print(f"   DSE will resume from previous state")
            else:
                logger.print(f"📁 No checkpoint found, starting fresh exploration")
        else:
            logger.print(f"🆕 Starting fresh exploration (checkpoint resuming disabled)")
        
        logger.print(f"\n🔄 Starting DSE exploration...")
        
        # Run DSE exploration with parameterized values
        results = explorer.run_exploration(
            max_iterations=MAX_ITERATIONS, 
            output_dir=OUTPUT_DIR, 
            timeout_min=TIMEOUT_MINUTES
        )
        
        logger.print(f"\n🎉 DSE completed successfully with {len(results)} iterations!")
        
        # Analyze results
        analyze_results(results, TARGET_II)
        
        # Show output files
        show_output_files(OUTPUT_DIR)
            
    except KeyboardInterrupt:
        logger.print("\n⚠️ DSE interrupted by user")
        logger.print(f"💾 State has been saved to: {OUTPUT_DIR}")
        logger.print(f"🔄 You can resume by running the script again")
        
    except FileNotFoundError as e:
        if "vitis_hls" in str(e):
            logger.print(f"\n❌ Vitis HLS tools not found")
            logger.print(f"Please ensure Vitis HLS is installed and in PATH")
            logger.print(f"You may need to run: source env.sh")
        else:
            logger.print(f"\n❌ File/directory not found: {e}")
            logger.print(f"Please check the TEMPLATE_DIR and OUTPUT_DIR paths")
            
    except Exception as e:
        logger.print(f"\n❌ DSE failed with error: {e}")
        if VERBOSE_LOGGING:
            import traceback
            logger.print(traceback.format_exc())
    
    finally:
        # Close the logger
        if logger:
            logger.close()


def analyze_results(results, target_ii):
    """Analyze and display DSE results."""
    global logger
    logger.print("\n" + "=" * 60)
    logger.print("📊 RESULTS ANALYSIS")
    logger.print("=" * 60)
    
    successful_runs = 0
    failed_runs = 0
    best_ii = float('inf')
    best_iteration = None
    best_metrics = None
    
    logger.print(f"\n📋 Iteration Details:")
    for result in results:
        status = result['status']
        iteration = result['iter']
        
        if status == "success":
            successful_runs += 1
            status_icon = "✅"
            
            # Extract and display metrics
            ii_value = "N/A"
            if 'metrics' in result and 'metrics' in result['metrics']:
                metrics = result['metrics']['metrics']
                if 'FFT_TOP' in metrics:
                    ii_value = metrics['FFT_TOP'].get('PipelineInitiationInterval', 'N/A')
                    
                    # Track best result
                    if isinstance(ii_value, (int, float)) and ii_value < best_ii:
                        best_ii = ii_value
                        best_iteration = iteration
                        best_metrics = metrics['FFT_TOP']
            
            logger.print(f"  {status_icon} Iteration {iteration:2d}: SUCCESS (FFT_TOP II = {ii_value})")
        else:
            failed_runs += 1
            status_icon = "❌"
            logger.print(f"  {status_icon} Iteration {iteration:2d}: {status.upper()}")
    
    # Summary statistics
    total_runs = successful_runs + failed_runs
    success_rate = (successful_runs / total_runs * 100) if total_runs > 0 else 0
    
    logger.print(f"\n📈 Summary Statistics:")
    logger.print(f"  Total iterations:     {total_runs}")
    logger.print(f"  Successful syntheses: {successful_runs}")
    logger.print(f"  Failed syntheses:     {failed_runs}")
    logger.print(f"  Success rate:         {success_rate:.1f}%")
    
    # Best result analysis
    if best_iteration is not None:
        logger.print(f"\n🏆 Best Result:")
        logger.print(f"  Iteration: {best_iteration}")
        logger.print(f"  FFT_TOP II: {best_ii}")
        
        if best_ii <= target_ii:
            logger.print(f"  🎯 TARGET ACHIEVED! (II {best_ii} ≤ target {target_ii})")
        else:
            improvement = ((best_ii - target_ii) / target_ii) * 100
            logger.print(f"  🎯 Target missed by {improvement:.1f}% (II {best_ii} > target {target_ii})")
        
        # Show additional metrics if available
        if best_metrics and VERBOSE_LOGGING:
            logger.print(f"\n  Additional metrics for best result:")
            for key, value in best_metrics.items():
                if key != 'PipelineInitiationInterval':
                    logger.print(f"    {key}: {value}")
    else:
        logger.print(f"\n❌ No successful syntheses achieved")


def show_output_files(output_dir):
    """Display information about generated output files."""
    global logger
    logger.print(f"\n📁 Output Files (in {output_dir}):")
    
    output_path = Path(output_dir)
    
    # Check for main result files
    if (output_path / "pragma_state.txt").exists():
        logger.print(f"  📄 pragma_state.txt     (human-readable exploration log)")
    
    if (output_path / "pragma_state.pkl").exists():
        logger.print(f"  💾 pragma_state.pkl     (checkpoint for resuming)")
    
    if (output_path / "explore.log").exists():
        logger.print(f"  📋 explore.log          (detailed test execution log)")

    if (output_path / "dse_trace.log").exists():
        logger.print(f"  📋 dse_trace.log          (detailed DSE decision trace log)")  

    # Count variant and work directories
    variant_dirs = list(output_path.glob("variant_*"))
    work_dirs = list(output_path.glob("work_*"))
    
    if variant_dirs:
        logger.print(f"  📦 {len(variant_dirs):2d} variant directories  (generated design variants)")
    
    if work_dirs:
        logger.print(f"  🔧 {len(work_dirs):2d} work directories     (synthesis results & logs)")
    
    # Show total disk usage if output directory exists
    if output_path.exists():
        try:
            total_size = sum(f.stat().st_size for f in output_path.rglob('*') if f.is_file())
            size_mb = total_size / (1024 * 1024)
            logger.print(f"  💿 Total size: {size_mb:.1f} MB")
        except:
            pass  # Ignore errors in size calculation


if __name__ == "__main__":
    main()