#!/usr/bin/env python3
import numpy as np
from pathlib import Path

def analyze_input_dirs(dirs):
    """Scan input .npz files for MAX_HITS, MAX_EDGES, NUM_LAYERS, NUM_EVENTS."""
    max_hits = 0
    max_edges = 0
    max_layers = 0
    num_events = 0

    for base in dirs:
        for npz_path in Path(base).rglob("*.npz"):
            num_events += 1
            data = np.load(npz_path, allow_pickle=False)

            # hits = number of rows in hit_cartesian
            if "hit_cartesian" in data.files:
                hits = data["hit_cartesian"].shape[0]
                print("hits", hits)
                max_hits = max(max_hits, hits)

            # edges = number of rows in edge_index
            if "edge_index" in data.files:
                edges = data["edge_index"].shape[1]
                max_edges = max(max_edges, edges)

            # layers = unique layer_id count
            if "layer_id" in data.files:
                layers = np.unique(data["layer_id"]).size
                max_layers = max(max_layers, layers)

    return max_hits, max_edges, max_layers, num_events

def analyze_output_dirs(dirs):
    """Scan output .npz files for MAX_TRACKS."""
    max_tracks = 0
    for base in dirs:
        for npz_path in Path(base).rglob("*.npz"):
            data = np.load(npz_path, allow_pickle=False)
            print(npz_path)
            # track count = first dimension of any per-track array, e.g. n_pixels
            if "n_pixels" in data.files:
                tracks = data["n_pixels"].shape[0]
                print("tracks", tracks)
                max_tracks = max(max_tracks, tracks)
    return max_tracks

if __name__ == "__main__":
    input_dirs = [
        "/nethome/sbommu3/FPGA/project_4/FPGA_Topic2_LU/2025_Spring/topic2/input_graphs/nontrigger/0",
        "/nethome/sbommu3/FPGA/project_4/FPGA_Topic2_LU/2025_Spring/topic2/input_graphs/trigger/1",
    ]
    output_dirs = [
        "/nethome/sbommu3/FPGA/project_4/FPGA_Topic2_LU/2025_Spring/topic2/outputs/nontrigger/",
        #"/nethome/sbommu3/FPGA/project_4/FPGA_Topic2_LU/2025_Spring/topic2/outputs/trigger/",
    ]

    max_hits, max_edges, max_layers, num_events = analyze_input_dirs(input_dirs)
    max_tracks = analyze_output_dirs(output_dirs)

    consts = {
        "MAX_HITS":       max_hits,
        "MAX_EDGES":      max_edges,
        "MAX_TRACKS":     max_tracks,
        "MAX_TRACK_SIZE": max_hits,
        "NUM_LAYERS":     max_layers,
        "NUM_EVENTS":     num_events,
    }

    print("// auto-generated constants:")
    for name, val in consts.items():
        print(f"#define {name:<15} {val}")
