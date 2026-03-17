import argparse
import shutil
import sys
from pathlib import Path

from hlsfactory.autodse_vitis.flow_autodse_vitis import AutoDSEFlow
from hlsfactory.framework import (
    Design,
    DesignDataset,
    count_total_designs_in_dataset_collection,
)
from hlsfactory.utils import DirSource, get_work_dir, remove_and_make_new_dir_if_exists

# Benchmarks matching local_multi_launch.sh (subset for demo; expand as needed)
DEFAULT_BENCHMARKS = ["2mm", "3mm", "syr2k", "gesummv", "atax-medium"]


def main() -> None:
    parser = argparse.ArgumentParser(
        description="Run AutoDSE LevelHeap DFS on autodse-vitis benchmarks",
    )
    parser.add_argument(
        "--benchmarks",
        nargs="+",
        default=DEFAULT_BENCHMARKS,
        help=f"Benchmark names (default: {DEFAULT_BENCHMARKS})",
    )
    parser.add_argument(
        "--max-iterations",
        type=int,
        default=3,
        help="Max DSE iterations per design (default: 3)",
    )
    parser.add_argument(
        "--timeout-min",
        type=int,
        default=10,
        help="Synthesis timeout in minutes (default: 10)",
    )
    parser.add_argument(
        "--n-jobs",
        type=int,
        default=1,
        help="Parallel jobs (default: 1)",
    )
    args = parser.parse_args()

    database_dir = (
        Path(__file__).resolve().parent.parent.parent
        / "hlsfactory"
        / "autodse_vitis"
        / "database"
        / "hlsyn-vivado-target"
    )
    if not database_dir.exists():
        print(f"❌ Database not found: {database_dir}")
        sys.exit(1)

    work_dir_top = get_work_dir(dir_source=DirSource.ENVFILE)
    work_dir = work_dir_top / "demo_autodse_flow"
    remove_and_make_new_dir_if_exists(work_dir)

    # Build dataset from requested benchmarks (only those that exist)
    dataset_name = "autodse_benchmarks"
    dataset_dir = work_dir / dataset_name
    dataset_dir.mkdir(parents=True, exist_ok=True)
    designs = []
    for name in args.benchmarks:
        src = database_dir / name
        if not src.exists():
            print(f"⚠️  Skipping missing benchmark: {name}")
            continue
        dst = dataset_dir / name
        if dst.exists():
            shutil.rmtree(dst)
        shutil.copytree(src, dst)
        designs.append(Design(name, dst))

    if not designs:
        print("❌ No valid benchmarks found")
        sys.exit(1)

    dataset = DesignDataset(
        dataset_name, dataset_dir, sorted(designs, key=lambda d: d.name)
    )
    total = count_total_designs_in_dataset_collection({dataset_name: dataset})
    print(f"📁 Loaded {total} benchmarks: {[d.name for d in dataset.designs]}")

    flow = AutoDSEFlow(
        work_dir,
        output_folder_name="autodse",
        max_iterations=args.max_iterations,
        timeout_min=args.timeout_min,
    )

    timeout_sec = args.timeout_min * 60
    print(
        f"🔄 Running AutoDSE (max_iter={args.max_iterations}, timeout={args.timeout_min}min)..."
    )
    results = flow.execute_multiple_designs(
        dataset.designs,
        n_jobs=args.n_jobs,
        timeout=timeout_sec,
    )
    print(f"✅ Completed: {len(results)} designs processed")
    print(f"📂 Outputs in: {work_dir}")
    for d in dataset.designs:
        autodse_dir = d.dir / "autodse"
        if autodse_dir.exists():
            print(f"   {d.name}: {autodse_dir}")


if __name__ == "__main__":
    main()
