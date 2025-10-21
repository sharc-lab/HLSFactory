# HLS Tool Benchmarking and Regression Testing

This tutorial walks through the regression testing demo in `demos/demo_regression_testing`.
The workflow compares multiple versions of Xilinx Vitis HLS and Vivado,
captures quality-of-result (QoR) metrics for each design, and summarizes the
impact of tool upgrades using statistical tests.

The end goal is a repeatable benchmark harness that keeps your HLS flow honest:
it runs identical workloads across tool versions, exports structured metrics,
and generates analysis artifacts you can track over time.

## Prerequisites

- Set `HLSFACTORY_WORK_DIR` in your shell or `.env` file. The demo writes all
  intermediate data under this directory.
- Install at least two Vitis HLS/Vivado versions and expose their install roots
  through the following environment variables before running the demo:
  `HLSFACTORY_VITIS_HLS_PATH_2021_1`, `HLSFACTORY_VITIS_HLS_PATH_2023_1`,
  `HLSFACTORY_VIVADO_PATH_2021_1`, and `HLSFACTORY_VIVADO_PATH_2023_1`.
- Ensure the datasets shipped with HLSFactory are accessible (see
  [](./custom_designs.md) if you would like to craft bespoke datasets first).

## Step 1: Prepare the Work Directory and Datasets

The driver script sets up a clean workspace, builds the built-in datasets, and
performs random sampling through the OptDSL frontend to keep the run time under
control.

```python
from pathlib import Path
from hlsfactory.utils import (
    DirSource,
    get_work_dir,
    remove_and_make_new_dir_if_exists,
)
from hlsfactory.datasets_builtin import datasets_builder
from hlsfactory.opt_dsl_frontend import OptDSLFrontend

WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_regression_testing"
remove_and_make_new_dir_if_exists(WORK_DIR)

datasets = datasets_builder(
    WORK_DIR,
    ["polybench", "machsuite", "chstone"],
    dataset_labels=["polybench_xilinx", "machsuite_xilinx", "chstone_xilinx"],
)

opt_dsl_frontend = OptDSLFrontend(
    WORK_DIR,
    random_sample=True,
    random_sample_num=16,
    random_sample_seed=64,
    log_execution_time=True,
)

datasets_post_frontend = (
    opt_dsl_frontend.execute_multiple_design_datasets_fine_grained_parallel(
        datasets,
        True,
        lambda name: f"{name}_post_frontend",
        n_jobs=32,
        cpu_affinity=list(range(32)),
    )
)
```

At the end of this stage you have identically sampled design sets ready for
side-by-side evaluation across tool versions.

## Step 2: Configure Tool Versions

Populate a dictionary of tool installations keyed by version labels. The demo
expects separate Vitis HLS and Vivado roots for at least two releases.

```python
from pathlib import Path
import os

VITIS_HLS_PATHS = {
    "2021_1": Path(os.environ["HLSFACTORY_VITIS_HLS_PATH_2021_1"]),
    "2023_1": Path(os.environ["HLSFACTORY_VITIS_HLS_PATH_2023_1"]),
}

VIVADO_PATHS = {
    "2021_1": Path(os.environ["HLSFACTORY_VIVADO_PATH_2021_1"]),
    "2023_1": Path(os.environ["HLSFACTORY_VIVADO_PATH_2023_1"]),
}

VITIS_HLS_BINS = {
    version: path / "bin" / "vitis_hls"
    for version, path in VITIS_HLS_PATHS.items()
}

DATASET_VERSIONS = {
    year: {
        dataset_name: dataset.copy_and_rename(
            f"{dataset_name}_post_hls_synth__{year}",
            WORK_DIR,
        )
        for dataset_name, dataset in datasets_post_frontend.items()
    }
    for year, _ in VITIS_HLS_BINS.items()
}
```

With these dictionaries in place the script clones each dataset per tool
version so that results do not overwrite each other.

## Step 3: Run Vitis HLS Synthesis for Each Version

Loop over every configured tool version, instantiate `VitisHLSSynthFlow`, and
launch the synthesis jobs in parallel. A five-minute timeout ensures hung jobs
do not stall the entire regression.

```python
from hlsfactory.flow_vitis import VitisHLSSynthFlow

TIMEOUT_HLS_SYNTH = 60.0 * 5  # 5 minutes
for vitis_version, dataset_map in DATASET_VERSIONS.items():
    vitis_bin = VITIS_HLS_BINS[vitis_version]
    toolflow_vitis_hls_synth = VitisHLSSynthFlow(
        vitis_hls_bin=str(vitis_bin),
        env_var_xilinx_hls=str(VITIS_HLS_PATHS[vitis_version]),
        env_var_xilinx_vivado=str(VIVADO_PATHS[vitis_version]),
    )
    toolflow_vitis_hls_synth.execute_multiple_design_datasets_fine_grained_parallel(
        dataset_map,
        False,
        lambda name: f"{name}_post_hls_synth__{vitis_version}",
        n_jobs=32,
        cpu_affinity=list(range(32)),
        timeout=TIMEOUT_HLS_SYNTH,
    )
```

Each run deposits `data_design.json`, `data_hls.json`, and
`execution_time_data.json` files under
`$HLSFACTORY_WORK_DIR/demo_regression_testing/<dataset>/<design>__<version>/`.
These JSON artifacts contain the metrics used in the analysis stage.

## Step 4: Execute the Regression Driver

You can kick off the whole pipeline via the provided script:

```bash
python demos/demo_regression_testing/regression_testing.py
```

Monitor the console output to ensure both tool versions complete successfully.
If you expand to additional tool versions, extend the dictionaries above and
the script will automatically pick them up.

## Step 5: Analyze and Summarize Results

Once synthesis finishes, run the companion analysis script to compute geometric
means, medians, and Wilcoxon signed-rank tests across the paired results:

```bash
python demos/demo_regression_testing/regression_testing_analysis.py
```

The analysis does four main things:

1. **Aggregate paired tool metrics** so only comparable design results remain.
2. **Compute geometric-mean ratios and medians** to quantify relative QoR shifts.
3. **Run Wilcoxon signed-rank tests** to spot statistically significant changes.
4. **Persist tables** for downstream reporting (CSV + LaTeX).

```python
datasets = list(WORK_DIR.glob("*__*"))

design_data = []
for dataset in datasets:
    version = dataset.name.split("__")[1]
    for design_fp in dataset.glob("*"):
        design_data.append(
            {
                "design_id": design_fp.name,
                "design_dir": design_fp,
                "version": version,
            },
        )
# ...
df_design_data = df_design_data[
    df_design_data["design_id"].map(df_design_data["design_id"].value_counts()) == 2
]
```

The script filters out incomplete results, ensuring each `design_id` has data
from both tool versions before deeper analysis proceeds.

```python
columns_to_compute_ratios = [
    "vitis_hls_dt",
    "latency_best_cycles",
    "latency_average_cycles",
    "latency_worst_cycles",
    "resources_lut_used",
    "resources_ff_used",
]

df_ratio = df_version_1[columns_to_compute_ratios].reset_index(drop=True) / (
    df_version_0[columns_to_compute_ratios].reset_index(drop=True)
)
ratio_data = {}
for column in columns_to_compute_ratios:
    data_column = df_ratio[column].dropna()
    ratio_average = gmean(data_column)
    ratio_percent_change = (ratio_average - 1) * 100
    # ...
    ratio_data[column] = {
        "ratio_average": ratio_average,
        "ratio_average_percent_change": ratio_percent_change,
        # ...
    }
```

Here geometric means and percent changes capture relative QoR shifts between
tool versions while remaining robust to outliers.

```python
test_data = {}
for column in columns_to_compute_ratios:
    data_column_version_0 = df_version_0[column].reset_index(drop=True)
    data_column_version_1 = df_version_1[column].reset_index(drop=True)
    df_combined = pd.DataFrame(
        {"version_0": data_column_version_0, "version_1": data_column_version_1},
    ).dropna()

    _, p_less = wilcoxon(
        df_combined["version_0"].to_numpy(),
        df_combined["version_1"].to_numpy(),
        alternative="less",
    )
    # ...
    test_data[column] = {
        "p_less": p_less,
        "p_greater": p_greater,
        "p_two_sided": p_two_sided,
        "n_samples": n_samples,
    }
```

Wilcoxon signed-rank tests quantify whether the newer tool release improves,
regresses, or simply differs from the baseline with statistical confidence.

```python
df_data_table = pd.DataFrame(data_table_flat)
df_data_table.to_csv(DATA_DIR / "regression_testing_stats.csv", index=False)
df_data_table.to_latex(
    DATA_DIR / "regression_testing_stats.txt",
    index=False,
)
```

Finally, the code exports machine-readable CSVs and LaTeX tables so you can
track regressions in dashboards, notebooks, or formal reports.

Outputs include:

- `demos/demo_regression_testing/data/regression_testing_stats.csv` – structured
  CSV with ratio, percent change, medians, and p-values per metric.
- `demos/demo_regression_testing/data/regression_testing_stats.txt` – LaTeX
  table export for quick inclusion in reports.

For quick exploration in Python:

```python
import pandas as pd

df_stats = pd.read_csv(
    "demos/demo_regression_testing/data/regression_testing_stats.csv"
)
print(df_stats[["feature", "ratio_average_percent_change", "p_two_sided"]])
```

## Tips for Extending the Benchmark

- Add more Vitis/Vivado releases by extending the environment variable sets and
  dictionary definitions. The rest of the pipeline adapts automatically.
- Integrate additional datasets or custom designs by reusing the dataset
  creation APIs covered in [](./custom_designs.md).
- Feed the generated CSVs into your preferred dashboarding tool or version
  control the outputs to build a regression history over time.

This demo provides a solid foundation for continuous verification of HLS tool
updates, complementing the end-to-end flow tutorials such as
[](./xilinx_flow.md) and [](./intel_flow.md).
