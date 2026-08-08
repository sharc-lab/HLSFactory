"""Report design counts under hlsfactory/hls_dataset_sources.

A directory is counted as a design when it contains an ``hlsfactory.toml`` file.
Also reports C/C++/header code lines via ``cloc``.
"""

from __future__ import annotations

import re
import shutil
import subprocess
from pathlib import Path

DESIGN_CONFIG_FILENAME = "hlsfactory.toml"

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
    print(f"{'dataset':<40} {'designs':>8} {'cxx_loc':>10}")
    print("-" * 60)

    rows: list[tuple[str, int, int]] = []
    for dataset_dir in dataset_dirs:
        count = len(count_designs(dataset_dir))
        loc = count_cxx_lines(dataset_dir)
        rows.append((dataset_dir.name, count, loc))

    rows.sort(key=lambda row: row[1], reverse=True)

    total_designs = 0
    total_loc = 0
    for name, count, loc in rows:
        total_designs += count
        total_loc += loc
        print(f"{name:<40} {count:>8} {loc:>10}")

    print("-" * 60)
    print(f"{'TOTAL':<40} {total_designs:>8} {total_loc:>10}")
    print(f"\nDatasets: {len(dataset_dirs)}")


if __name__ == "__main__":
    main()
