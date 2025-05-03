#!/usr/bin/env python3
import numpy as np
from pathlib import Path
import argparse
import os

def write_array_to_bin(array: np.ndarray, filename: Path):
    """
    Flatten and write a numpy array to a .bin file, converting
    bool→uint8, ints→int32, floats→float32.
    """
    array = np.asarray(array)
    if array.dtype == np.bool_:
        array = array.astype(np.uint8)
    if np.issubdtype(array.dtype, np.integer):
        array = array.astype(np.int32)
    else:
        array = array.astype(np.float32)
    with open(filename, 'wb') as f:
        f.write(array.tobytes())

def convert_npz_to_bin(npz_path: Path, out_dir: Path):
    """
    Load all arrays from an .npz archive and write each one
    to <out_dir>/<key>.bin
    """
    data = np.load(npz_path, allow_pickle=False)
    out_dir.mkdir(parents=True, exist_ok=True)
    print(f"\nConverting {npz_path} → {out_dir}/")
    for key in data.files:
        arr = data[key]
        out_file = out_dir / f"{key}.bin"
        write_array_to_bin(arr, out_file)
        print(f"  • {key} [{arr.dtype}, {arr.shape}] → {out_file}")

def main():
    parser = argparse.ArgumentParser(
        description="Recursively convert all .npz under one or more INPUT_DIRs into per-event .bin folders.")
    parser.add_argument(
        "input_dirs",
        nargs="+",
        type=Path,
        help="One or more root directories to scan for .npz files"
    )
    parser.add_argument(
        "-o", "--output-root",
        type=Path,
        default=Path("converted"),
        help="Base directory under which to mirror each event's .bin output"
    )

    args = parser.parse_args()

    for input_root in args.input_dirs:
        if not input_root.exists():
            print(f"⚠️  Skipping non-existent folder: {input_root}")
            continue

        for npz_path in input_root.rglob("*.npz"):
            # build a mirror path under output_root, e.g.
            # input_graphs/trigger/1/event12345.npz
            #    → converted/trigger/1/event12345/
            rel = npz_path.relative_to(input_root).with_suffix("")  # strip .npz
            out_dir = args.output_root / input_root.name / rel
            convert_npz_to_bin(npz_path, out_dir)

if __name__ == "__main__":
    main()
