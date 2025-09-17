#!/usr/bin/env python3
"""
DSE Test Script with Different Objective Functions
This script demonstrates how to use different objective functions with the DSE explorer.
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
from objective.objectives import (
    TargetIIObjective, 
    ResourceConstrainedLatencyObjective,
    ResourceConstrainedIILatencyObjective
)

# ===================================================================
# CONFIGURATION PARAMETERS
# ===================================================================

# Path Configuration
# TEMPLATE_DIR = "/home/cy/github/DSE-HLS-FFT/database/n256_UF4"
# BASE_OUTPUT_DIR = "/home/cy/github/DSE-HLS-FFT/0_work_dir"

TEMPLATE_DIR = "/home/cy/github/DSE-HLS-FFT/database/vitishlssyn/3mm"
BASE_OUTPUT_DIR = "/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_vitishlssyn_3mm"

# DSE Parameters
MAX_ITERATIONS = 3
TIMEOUT_MINUTES = 10
RESUME_FROM_CHECKPOINT =  False
VERBOSE_LOGGING = True

# ===================================================================
# OBJECTIVE FUNCTION CONFIGURATIONS
# ===================================================================

def create_objective_configs():
    """Create different objective function configurations to test."""
    return [
        # {
        #     "name": "target_ii_1", 
        #     "description": "Target II ≤ 1",
        #     "objective": TargetIIObjective(target_ii=1),
        #     "output_dir": f"{BASE_OUTPUT_DIR}/dse_target_ii_1"
        # },
        {
            "name": "target_ii_32",
            "description": "Target II ≤ 32", 
            "objective": TargetIIObjective(target_ii=32),
            "output_dir": f"{BASE_OUTPUT_DIR}/dse_target_ii_32"
        },
        {
            "name": "resource_constrained_latency",
            "description": "Minimize latency (DSP≤5130, BRAM≤3240, LUT≤886680, FF≤1773360, URAM≤720)",
            "objective": ResourceConstrainedLatencyObjective(
                max_dsp=5130, 
                max_bram=3240, 
                max_lut=886680,
                max_ff=1773360,
                max_uram=720
            ),
            "output_dir": f"{BASE_OUTPUT_DIR}/dse_resource_latency"
        },
        # {
        #     "name": "resource_constrained_ii_latency",
        #     "description": "Minimize II+latency (DSP≤200, BRAM≤100, LUT≤20000, FF≤25000, URAM≤50)",
        #     "objective": ResourceConstrainedIILatencyObjective(
        #         max_dsp=200,
        #         max_bram=100,
        #         max_lut=20000,
        #         max_ff=25000,
        #         max_uram=50,
        #         ii_weight=0.6,
        #         latency_weight=0.4
        #     ),
        #     "output_dir": f"{BASE_OUTPUT_DIR}/dse_resource_ii_latency"
        # }
    ]


def run_dse_with_objective(config):
    """Run DSE with a specific objective configuration."""
    output_dir = config["output_dir"]
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
        logger.print(f"🚀 DSE Test: {config['name']}")
        logger.print("=" * 70)
        logger.print(f"Objective: {config['description']}")
        logger.print(f"Template: {TEMPLATE_DIR}")
        logger.print(f"Output: {output_dir}")
        logger.print(f"Max iterations: {MAX_ITERATIONS}")
        logger.print(f"Timeout: {TIMEOUT_MINUTES} minutes")
        logger.print("=" * 70)
        
        # Create container and explorer
        container = TemplateContainer(TEMPLATE_DIR)
        explorer = DSEExplorer(
            container=container,
            objective_fn=config["objective"]
        )
        
        # Set logger
        explorer.set_logger(logger)
        
        logger.print(f"✅ Template loaded: {len(explorer.pragma_locations)} pragma locations")
        logger.print(f"✅ Explorer initialized with objective: {config['objective'].get_target_description()}")
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
        
        logger.print(f"\n🔄 Starting DSE exploration...")
        
        # Run DSE
        results = explorer.run_exploration(
            max_iterations=MAX_ITERATIONS,
            output_dir=output_dir,
            timeout_min=TIMEOUT_MINUTES
        )
        
        logger.print(f"\n🎉 DSE completed: {len(results)} iterations!")
        
        # Analyze results
        analyze_results(results, config, logger)
        
        return True, len(results)
        
    except KeyboardInterrupt:
        logger.print("\n⚠️ DSE interrupted by user")
        return False, 0
        
    except Exception as e:
        logger.print(f"\n❌ DSE failed: {e}")
        if VERBOSE_LOGGING:
            import traceback
            logger.print(traceback.format_exc())
        return False, 0
        
    finally:
        logger.close()


def analyze_results(results, config, logger):
    """Analyze and display results for a specific objective."""
    logger.print("\n" + "=" * 50)
    logger.print("📊 RESULTS ANALYSIS")
    logger.print("=" * 50)
    
    successful_runs = 0
    failed_runs = 0
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
                
                # Try to extract a meaningful score for comparison
                score = None
                if 'combined_score' in obj_details:
                    score = obj_details['combined_score']
                elif 'current_ii' in obj_details:
                    score = obj_details['current_ii']
                elif 'latency' in obj_details:
                    score = obj_details['latency']
                
                if score is not None and score < best_score:
                    best_score = score
                    best_iteration = iteration
                    best_details = obj_details
                
                logger.print(f"  {status_icon} Iteration {iteration:2d}: {obj_result} (score: {score})")
                
                # Show key metrics
                if 'current_ii' in obj_details:
                    logger.print(f"     Current II: {obj_details['current_ii']}")
                if 'latency' in obj_details:
                    logger.print(f"     Latency: {obj_details['latency']}")
                if 'resource_violations' in obj_details and obj_details['resource_violations']:
                    logger.print(f"     Resource violations: {obj_details['resource_violations']}")
            else:
                logger.print(f"  {status_icon} Iteration {iteration:2d}: SUCCESS (no objective details)")
        else:
            failed_runs += 1
            status_icon = "❌"
            logger.print(f"  {status_icon} Iteration {iteration:2d}: {status.upper()}")
    
    # Summary statistics
    total_runs = successful_runs + failed_runs
    success_rate = (successful_runs / total_runs * 100) if total_runs > 0 else 0
    
    logger.print(f"\n📈 Summary:")
    logger.print(f"  Total iterations: {total_runs}")
    logger.print(f"  Successful: {successful_runs}")
    logger.print(f"  Failed: {failed_runs}")
    logger.print(f"  Success rate: {success_rate:.1f}%")
    
    # Best result
    if best_iteration is not None:
        logger.print(f"\n🏆 Best Result (Iteration {best_iteration}):")
        logger.print(f"  Score: {best_score}")
        if best_details:
            for key, value in best_details.items():
                if key not in ['bottleneck_module']:  # Skip internal details
                    logger.print(f"  {key}: {value}")


def main():
    """Run DSE tests with different objective functions."""
    print("🚀 DSE Multi-Objective Test Suite")
    print("=" * 80)
    
    objective_configs = create_objective_configs()
    
    print(f"Testing {len(objective_configs)} different objective functions:")
    for i, config in enumerate(objective_configs, 1):
        print(f"  {i}. {config['name']}: {config['description']}")
    print()
    
    # Ask user which objectives to run
    print("Enter objective numbers to run (e.g., '1,3' or 'all' for all):")
    user_input = input("> ").strip().lower()
    
    if user_input == 'all':
        selected_configs = objective_configs
    else:
        try:
            indices = [int(x.strip()) - 1 for x in user_input.split(',')]
            selected_configs = [objective_configs[i] for i in indices if 0 <= i < len(objective_configs)]
        except (ValueError, IndexError):
            print("❌ Invalid input. Running all objectives.")
            selected_configs = objective_configs
    
    if not selected_configs:
        print("❌ No valid objectives selected.")
        return
    
    print(f"\n🔄 Running {len(selected_configs)} objective tests...\n")
    
    # Run each selected objective
    results_summary = []
    for i, config in enumerate(selected_configs, 1):
        print(f"\n{'='*80}")
        print(f"🎯 TEST {i}/{len(selected_configs)}: {config['name']}")
        print(f"{'='*80}")
        
        success, iterations = run_dse_with_objective(config)
        results_summary.append({
            'name': config['name'],
            'description': config['description'],
            'success': success,
            'iterations': iterations,
            'output_dir': config['output_dir']
        })
        
        if success:
            print(f"✅ {config['name']} completed successfully ({iterations} iterations)")
        else:
            print(f"❌ {config['name']} failed")
    
    # Final summary
    print(f"\n{'='*80}")
    print("📊 FINAL SUMMARY")
    print(f"{'='*80}")
    
    for result in results_summary:
        status = "✅ SUCCESS" if result['success'] else "❌ FAILED"
        print(f"{status} {result['name']}: {result['description']}")
        print(f"   Output: {result['output_dir']}")
        if result['success']:
            print(f"   Iterations: {result['iterations']}")
        print()
    
    successful_tests = sum(1 for r in results_summary if r['success'])
    print(f"Overall: {successful_tests}/{len(results_summary)} tests completed successfully")


if __name__ == "__main__":
    main()