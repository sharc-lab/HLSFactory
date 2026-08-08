"""Report design counts under hlsfactory/hls_dataset_sources.

A directory is counted as a design when it contains an ``hlsfactory.toml`` file.
Also reports C/C++/header code lines via ``cloc``, and whether every design in a
dataset declares Vitis HLS csynth / csim / cosim flow support in its
``hlsfactory.toml``.
"""

from __future__ import annotations

import re
import shutil
import subprocess
from pathlib import Path

from hlsfactory.design_config import (
    DESIGN_CONFIG_FILENAME,
    FlowName,
    read_design_config,
)

REPO_ROOT = Path(__file__).resolve().parents[1]
DATASET_SOURCES = REPO_ROOT / "hlsfactory" / "hls_dataset_sources"

INCLUDE_DATASETS = [
    "accelerators",
    "auto_ntt",
    "chstone_new",
    "class_ece8893_2022_fall",
    "class_ece8893_2023_spring",
    "flowgnn",
    "forgebench",
    "hp_fft_hls",
    "machsuite",
    "polybench",
    "pp4fpgas",
    "rosetta",
    "soda",
    "spector_hls",
    "stream_hls",
    "vitis_examples",
    "c2hlsc",
]

CXX_LANGUAGES = frozenset({"C", "C++", "C Header", "C++ Header"})

# Example: | C                                  47   335.30 KB         995        2536        6186 |
_CLOC_ROW = re.compile(
    r"^\|\s*(?P<lang>.+?)\s+(?P<files>\d+)\s+"
    r"(?P<size>[\d.]+\s+\wB)\s+"
    r"(?P<blank>\d+)\s+(?P<comment>\d+)\s+(?P<code>\d+)\s*\|$"
)


def count_designs(dataset_dir: Path) -> list[Path]:
    """Return design directories under ``dataset_dir`` that have a config file."""
    return sorted(
        toml_path.parent
        for toml_path in dataset_dir.rglob(DESIGN_CONFIG_FILENAME)
        if toml_path.is_file()
    )


def count_cxx_lines(dataset_dir: Path) -> int:
    """Return C/C++/header code lines under ``dataset_dir`` using ``cloc``."""
    cloc = shutil.which("cloc")
    if cloc is None:
        raise SystemExit("cloc not found on PATH; install cloc to report cxx_loc")

    result = subprocess.run(
        [cloc, str(dataset_dir)],
        check=True,
        capture_output=True,
        text=True,
    )
    # Progress messages go to stdout on some builds; keep stdout+stderr for parsing.
    output = f"{result.stdout}\n{result.stderr}"

    total = 0
    for line in output.splitlines():
        match = _CLOC_ROW.match(line.strip())
        if not match:
            continue
        lang = match.group("lang").strip()
        if lang in CXX_LANGUAGES:
            total += int(match.group("code"))
    return total


def flow_support_counts(
    design_dirs: list[Path],
) -> tuple[int, int, int, bool, bool, bool]:
    """Return csynth/csim/cosim counts and all-* flags from hlsfactory.toml."""
    csynth_count = 0
    csim_count = 0
    cosim_count = 0
    for design_dir in design_dirs:
        config = read_design_config(design_dir / DESIGN_CONFIG_FILENAME)
        if config.supports_flow(FlowName.VITIS_HLS_SYNTH):
            csynth_count += 1
        if config.supports_flow(FlowName.VITIS_HLS_CSIM):
            csim_count += 1
        if config.supports_flow(FlowName.VITIS_HLS_COSIM):
            cosim_count += 1

    n = len(design_dirs)
    all_csynth = n > 0 and csynth_count == n
    all_csim = n > 0 and csim_count == n
    all_cosim = n > 0 and cosim_count == n
    return csynth_count, csim_count, cosim_count, all_csynth, all_csim, all_cosim


def _yn(value: bool) -> str:
    return "yes" if value else "no"


def main() -> None:
    if not DATASET_SOURCES.is_dir():
        raise SystemExit(f"Dataset sources not found: {DATASET_SOURCES}")

    dataset_dirs = []
    for name in INCLUDE_DATASETS:
        dataset_dir = DATASET_SOURCES / name
        if not dataset_dir.is_dir():
            raise SystemExit(f"Included dataset not found: {dataset_dir}")
        dataset_dirs.append(dataset_dir)

    print(f"Scanning: {DATASET_SOURCES}")
    header = (
        f"{'dataset':<40} {'designs':>8} {'cxx_loc':>10} "
        f"{'csynth':>10} {'all_csynth':>10} "
        f"{'csim':>10} {'all_csim':>8} "
        f"{'cosim':>10} {'all_cosim':>9}"
    )
    print(header)
    print("-" * len(header))

    rows: list[tuple[str, int, int, int, bool, int, bool, int, bool]] = []
    for dataset_dir in dataset_dirs:
        designs = count_designs(dataset_dir)
        count = len(designs)
        loc = count_cxx_lines(dataset_dir)
        (
            csynth_n,
            csim_n,
            cosim_n,
            all_csynth,
            all_csim,
            all_cosim,
        ) = flow_support_counts(designs)
        rows.append(
            (
                dataset_dir.name,
                count,
                loc,
                csynth_n,
                all_csynth,
                csim_n,
                all_csim,
                cosim_n,
                all_cosim,
            )
        )

    rows.sort(key=lambda row: row[1], reverse=True)

    total_designs = 0
    total_loc = 0
    total_csynth = 0
    total_csim = 0
    total_cosim = 0
    for (
        name,
        count,
        loc,
        csynth_n,
        all_csynth,
        csim_n,
        all_csim,
        cosim_n,
        all_cosim,
    ) in rows:
        total_designs += count
        total_loc += loc
        total_csynth += csynth_n
        total_csim += csim_n
        total_cosim += cosim_n
        csynth_frac = f"{csynth_n}/{count}"
        csim_frac = f"{csim_n}/{count}"
        cosim_frac = f"{cosim_n}/{count}"
        print(
            f"{name:<40} {count:>8} {loc:>10} "
            f"{csynth_frac:>10} {_yn(all_csynth):>10} "
            f"{csim_frac:>10} {_yn(all_csim):>8} "
            f"{cosim_frac:>10} {_yn(all_cosim):>9}"
        )

    print("-" * len(header))
    total_csynth_frac = f"{total_csynth}/{total_designs}"
    total_csim_frac = f"{total_csim}/{total_designs}"
    total_cosim_frac = f"{total_cosim}/{total_designs}"
    print(
        f"{'TOTAL':<40} {total_designs:>8} {total_loc:>10} "
        f"{total_csynth_frac:>10} {'':>10} "
        f"{total_csim_frac:>10} {'':>8} "
        f"{total_cosim_frac:>10}"
    )
    print(f"\nDatasets: {len(dataset_dirs)}")


if __name__ == "__main__":
    main()
