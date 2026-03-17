# Loading Custom Designs and Datasets

This tutorial covers how to load your own HLS designs and organize them into datasets for use with HLSFactory flows.

## Loading Built-in Datasets

To load curated datasets (PolyBench, MachSuite, CHStone, etc.) into your working directory:

```python
from hlsfactory.datasets_builtin import datasets_builder

datasets = datasets_builder(
    WORK_DIR,
    ["polybench", "machsuite"],
    dataset_labels=["my_polybench", "my_machsuite"],
)
```

`datasets` is a `DesignDatasetCollection` (a dictionary of `DesignDataset` objects) that you can pass to frontend and tool flows. See [HLS Design Collection](../built_in_datasets) for available keys.

## Loading a Single Custom Design

If you have a single design directory:

```python
from pathlib import Path
from hlsfactory.framework import Design

my_design = Design("my_design", Path("/path/to/design_dir"))
my_design = my_design.copy_to_new_parent_dir(WORK_DIR)
```

Use `copy_to_new_parent_dir` to avoid modifying source files; flows run on the copied design.

## Loading a Directory of Custom Designs

If you have a folder containing multiple design subdirectories:

```python
from hlsfactory.framework import DesignDataset

my_dataset = DesignDataset.from_dir(
    "my_dataset",
    Path("/path/to/dataset_folder"),
).copy_dataset(WORK_DIR)
```

Each subdirectory of `dataset_folder` is treated as one `Design`. The result can be passed to flows as `{"my_dataset": my_dataset}` or combined with other datasets.

## Design Requirements

Each design directory must contain the entry points required by the flows you run (e.g., `dataset_hls.tcl` and `dataset_hls_ip_export.tcl` for Xilinx flows). See the [Integrating Your HLS Project](../extending) section in the Extending guide for details.

For a full walkthrough with concrete examples, see the `demos/demo_custom_datasets` directory in the repository. For detailed API documentation, see [Designs and Design Datasets](../framework/designs_datasets).
