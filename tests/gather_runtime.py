import argparse
import json
import math
import statistics
from pathlib import Path


def percentile(n: list[int | float], percent: float, key=lambda x: x) -> float | None:
    """
    Find the percentile of a list of values.

    @parameter n - is a list of values. Note n MUST BE already sorted.
    @parameter percent - a float value from 0.0 to 1.0.
    @parameter key - optional key function to compute value from each element of n.

    @return - the percentile of the values
    """

    n = sorted(n)

    if not n:
        return None
    k = (len(n) - 1) * percent
    f = math.floor(k)
    c = math.ceil(k)
    if f == c:
        return key(n[int(k)])
    d0 = key(n[int(f)]) * (c - k)
    d1 = key(n[int(c)]) * (k - f)
    return d0 + d1


def main(args: argparse.Namespace) -> None:
    search_dir: Path = args.search_dir

    dirs = list(search_dir.rglob("**/dataset_hls.tcl"))
    design_dirs = [fp.parent for fp in dirs]

    print(f"Found {len(design_dirs)} design dirs")

    design_dirs_with_runtime_json = []
    for design_dir in design_dirs:
        runtime_json_fp = design_dir / "execution_time_data.json"
        if runtime_json_fp.exists():
            design_dirs_with_runtime_json.append(design_dir)

    print(f"Found {len(design_dirs_with_runtime_json)} design dirs with runtime json")

    flow_name = "VitisHLSSynthFlow"

    timeouts = []
    for design_dir in design_dirs:
        timeout_fp = design_dir / f"timeout__{flow_name}.txt"
        if timeout_fp.exists():
            timeouts.append(design_dir.name)

    print(f"Found {len(timeouts)} designs with {flow_name} timeouts")

    data = []
    for design_dir in design_dirs_with_runtime_json:
        # skip if timeout file exists
        timeout_fp = design_dir / f"timeout__{flow_name}.txt"
        if timeout_fp.exists():
            continue
        runtime_json_fp = design_dir / "execution_time_data.json"
        runtime_data = json.loads(runtime_json_fp.read_text())
        if flow_name in runtime_data:
            entry = {
                "design": design_dir.name,
                "runtime": runtime_data[flow_name]["dt"],
            }
            data.append(entry)

    print(
        f"Found {len(data)} designs with {flow_name} runtime data that did not timeout"
    )

    # compute average and median runtime
    runtimes = [entry["runtime"] for entry in data]
    avg_runtime = sum(runtimes) / len(runtimes)
    median_runtime = sorted(runtimes)[len(runtimes) // 2]

    print(f"Average {flow_name} runtime: {avg_runtime:.2f} seconds")
    print(f"Median {flow_name} runtime: {median_runtime:.2f} seconds")

    percentiles = {}
    for p in [5, 95]:
        percentiles[p] = percentile(runtimes, p / 100)

    print(f"5th percentile {flow_name} runtime: {percentiles[5]:.2f} seconds")
    print(f"95th percentile {flow_name} runtime: {percentiles[95]:.2f} seconds")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Gather dataset runtime info.")
    parser.add_argument(
        "search_dir",
        type=Path,
        help="Directory to search for dataset json files.",
    )

    args = parser.parse_args()
    main(args)
