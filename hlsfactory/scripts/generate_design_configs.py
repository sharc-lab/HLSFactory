"""Migration script to generate hlsfactory.toml files for existing designs.

This script scans all design directories under hls_dataset_sources/ and generates
hlsfactory.toml configuration files based on the detected TCL files.

Usage Examples:
    # Preview what would be created (recommended first step)
    uv run python -m hlsfactory.scripts.generate_design_configs --dry-run

    # Preview with verbose output showing each design
    uv run python -m hlsfactory.scripts.generate_design_configs --dry-run --verbose

    # Generate configs for all designs
    uv run python -m hlsfactory.scripts.generate_design_configs

    # Generate configs for a specific dataset only
    uv run python -m hlsfactory.scripts.generate_design_configs --dataset polybench

    # Generate configs for a specific dataset with preview
    uv run python -m hlsfactory.scripts.generate_design_configs \\
        --dataset machsuite --dry-run

TCL File Detection:
    The script automatically detects the following TCL files and maps them to flows:

    - dataset_hls.tcl          -> VitisHLSSynthFlow (synth_tcl)
    - dataset_hls_ip_export.tcl -> VitisHLSImplFlow (impl_tcl)
    - dataset_hls_csim.tcl     -> VitisHLSCsimFlow (csim_tcl)
    - dataset_hls_cosim_setup.tcl -> VitisHLSCosimSetupFlow (cosim_setup_tcl)
    - dataset_hls_cosim.tcl    -> VitisHLSCosimFlow (cosim_tcl)
    - opt_template.tcl         -> OptDSLv2 (opt_dsl_file)

Generated Config Format:
    design_name = "<directory_name>"
    dataset_name = "<parent_dataset_name>"
    tags = []

    [[flow_configs]]
    flow_name = "VitisHLSSynthFlow"
    synth_tcl = "dataset_hls.tcl"

    [[flow_configs]]
    flow_name = "OptDSLv2"
    opt_dsl_file = "opt_template.tcl"

Notes:
    - Existing hlsfactory.toml files are never overwritten (skipped)
    - Designs without any recognized TCL files generate warnings
    - Use --dry-run first to preview changes before generating
"""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

from hlsfactory.design_config import (
    DesignConfig,
    FlowConfig,
    FlowName,
    write_design_config,
)

# Map dataset directory names to dataset_name values
DATASET_NAME_MAP: dict[str, str] = {
    "polybench": "polybench",
    "polybench__reproducible": "polybench_reproducible",
    "machsuite": "machsuite",
    "chstone": "chstone",
    "pp4fpgas": "pp4fpgas",
    "vitis_examples": "vitis_examples",
    "accelerators": "accelerators",
    "soda": "soda",
    "hp_fft_hls": "hp_fft",
    "stream_hls": "stream_hls",
    "auto_ntt": "auto_ntt",
    "forgebench": "forgebench",
    "flowgnn": "flowgnn",
    "gnnbuilder": "gnnbuilder",
    "rosetta": "rosetta",
    "hlsyn": "hlsyn",
    "hlsyn-vitis": "hlsyn_vitis",
    "whetstone": "whetstone",
    "class_ece8893_2022_fall": "class_ece8893_2022_fall",
    "class_ece8893_2023_spring": "class_ece8893_2023_spring",
    "class_ece8893_spring_2025": "class_ece8893_spring_2025",
}

# Standard TCL file detection patterns
# Maps setting_key -> list of possible filenames to look for
TCL_PATTERNS: dict[str, list[str]] = {
    "synth_tcl": ["dataset_hls.tcl"],
    "impl_tcl": ["dataset_hls_ip_export.tcl"],
    "csim_tcl": ["dataset_hls_csim.tcl"],
    "cosim_setup_tcl": ["dataset_hls_cosim_setup.tcl"],
    "cosim_tcl": ["dataset_hls_cosim.tcl"],
    "opt_dsl_file": ["opt_template.tcl"],
}


def detect_tcl_files(design_dir: Path) -> dict[str, str]:
    """Detect which TCL files exist in a design directory.

    Returns:
        Dictionary mapping setting keys to found filenames.
    """
    found: dict[str, str] = {}
    for setting_key, patterns in TCL_PATTERNS.items():
        for pattern in patterns:
            if (design_dir / pattern).exists():
                found[setting_key] = pattern
                break
    return found


def generate_flow_configs(tcl_files: dict[str, str]) -> list[FlowConfig]:
    """Generate FlowConfig objects based on detected TCL files.

    Args:
        tcl_files: Dictionary mapping setting keys to filenames.

    Returns:
        List of FlowConfig objects for the detected flows.
    """
    flow_configs: list[FlowConfig] = []

    # VitisHLSSynthFlow
    if "synth_tcl" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.VITIS_HLS_SYNTH.value,
                flow_settings={"synth_tcl": tcl_files["synth_tcl"]},
            )
        )

    # VitisHLSImplFlow
    if "impl_tcl" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.VITIS_HLS_IMPL.value,
                flow_settings={"impl_tcl": tcl_files["impl_tcl"]},
            )
        )

    # VitisHLSCsimFlow
    if "csim_tcl" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.VITIS_HLS_CSIM.value,
                flow_settings={"csim_tcl": tcl_files["csim_tcl"]},
            )
        )

    # VitisHLSCosimSetupFlow
    if "cosim_setup_tcl" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.VITIS_HLS_COSIM_SETUP.value,
                flow_settings={"cosim_setup_tcl": tcl_files["cosim_setup_tcl"]},
            )
        )

    # VitisHLSCosimFlow
    if "cosim_tcl" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.VITIS_HLS_COSIM.value,
                flow_settings={"cosim_tcl": tcl_files["cosim_tcl"]},
            )
        )

    # OptDSLv2
    if "opt_dsl_file" in tcl_files:
        flow_configs.append(
            FlowConfig(
                flow_name=FlowName.OPT_DSL_V2.value,
                flow_settings={"opt_dsl_file": tcl_files["opt_dsl_file"]},
            )
        )

    return flow_configs


def process_design_directory(
    design_dir: Path,
    dataset_name: str,
    dry_run: bool = False,
) -> tuple[bool, str]:
    """Process a single design directory.

    Args:
        design_dir: Path to the design directory.
        dataset_name: Name of the parent dataset.
        dry_run: If True, don't write files, just show what would be done.

    Returns:
        Tuple of (was_created, status_message).
    """
    config_path = design_dir / "hlsfactory.toml"

    if config_path.exists():
        return False, f"SKIP: {design_dir.name} (config already exists)"

    tcl_files = detect_tcl_files(design_dir)
    if not tcl_files:
        return False, f"WARN: {design_dir.name} (no TCL files found)"

    flow_configs = generate_flow_configs(tcl_files)

    config = DesignConfig(
        design_name=design_dir.name,
        dataset_name=dataset_name,
        tags=[],
        env_vars={},
        flow_configs=flow_configs,
    )

    flows_str = ", ".join(fc.flow_name for fc in flow_configs)

    if dry_run:
        msg = f"DRY-RUN: Would create {config_path.name} with flows: [{flows_str}]"
        return True, msg

    write_design_config(config_path, config)
    return True, f"CREATED: {config_path.name} with flows: [{flows_str}]"


def main() -> int:
    """Main entry point for the migration script."""
    parser = argparse.ArgumentParser(
        description="Generate hlsfactory.toml files for existing designs"
    )
    parser.add_argument(
        "--dry-run",
        action="store_true",
        help="Show what would be done without making changes",
    )
    parser.add_argument(
        "--dataset",
        type=str,
        default=None,
        help="Process only a specific dataset (e.g., 'polybench')",
    )
    parser.add_argument(
        "--verbose",
        "-v",
        action="store_true",
        help="Show details for each design processed",
    )
    args = parser.parse_args()

    # Find the hls_dataset_sources directory
    script_dir = Path(__file__).resolve().parent
    dataset_sources = script_dir.parent / "hls_dataset_sources"

    if not dataset_sources.exists():
        print(f"ERROR: Dataset sources not found at {dataset_sources}")
        return 1

    created_count = 0
    skipped_count = 0
    warned_count = 0

    # Skip these directories as they are not design datasets
    skip_dirs = {"common", "__pycache__", "WIP", ".git"}

    for dataset_dir in sorted(dataset_sources.iterdir()):
        if not dataset_dir.is_dir():
            continue
        if dataset_dir.name.startswith(".") or dataset_dir.name in skip_dirs:
            continue
        if args.dataset and dataset_dir.name != args.dataset:
            continue

        dataset_name = DATASET_NAME_MAP.get(dataset_dir.name, dataset_dir.name)
        print(f"\nProcessing dataset: {dataset_dir.name} -> {dataset_name}")

        for design_dir in sorted(dataset_dir.iterdir()):
            if not design_dir.is_dir():
                continue
            if design_dir.name in skip_dirs:
                continue

            was_created, message = process_design_directory(
                design_dir, dataset_name, args.dry_run
            )

            if args.verbose or was_created or message.startswith("WARN"):
                print(f"  {message}")

            if was_created:
                created_count += 1
            elif message.startswith("WARN"):
                warned_count += 1
            else:
                skipped_count += 1

    print(
        f"\nSummary: {created_count} created, "
        f"{skipped_count} skipped, {warned_count} warnings"
    )
    return 0


if __name__ == "__main__":
    sys.exit(main())
