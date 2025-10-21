# Machine Learning for QoR Estimation

This tutorial describes the machine-learning quality-of-result (QoR) workflow
packaged under `demos/demo_ml`. The demo aggregates Xilinx implementation data,
derives additional features from HLS reports, and trains multi-output regressors
that forecast implementation utilization and timing directly from HLS reports.

The full pipeline consists of three scripts:

1. `ml_agg.py` gathers implementation artifacts and produces a compressed archive.
2. `ml_pre.py` augments the raw dataset with derived statistics extracted from
   HLS reports.
3. `ml_model.py` trains hist-gradient boosting models, benchmarks them against
   HLS-reported estimates, and saves diagnostic plots.

## Prerequisites

- You need a populated design workspace containing post-frontend designs and
  Vitis implementation artifacts. The datasets created in
  [](./xilinx_flow.md) or your own `DesignDataset` exports are suitable.
- Ensure the artifacts follow the layout expected by
  `DataAggregatorXilinx` (e.g., each design directory contains a `artifacts.zip`
  with the Vitis reports).
- Install the optional machine learning dependencies declared under the
  `ml` extra in `pyproject.toml` (`pip install .[ml]`).

## Step 1: Gather Implementation Data

Point the aggregation script at the directory containing your design-space
dataset. Adjust `test_data_dir` to match your workspace root (for example,
`Path(get_work_dir() / "demo_full_flow_xilinx")`).

```python
from pathlib import Path
from hlsfactory.data_packaging import DataAggregatorXilinx
from hlsfactory.framework import DesignDataset

test_data_dir = Path("/path/to/your/design_workspace")

dataset_polybench = DesignDataset.from_dir(
    "polybench_xilinx__post_frontend",
    test_data_dir / "polybench_xilinx__post_frontend",
)
dataset_machsuite = DesignDataset.from_dir(
    "machsuite_xilinx__post_frontend",
    test_data_dir / "machsuite_xilinx__post_frontend",
)
dataset_chstone = DesignDataset.from_dir(
    "chstone_xilinx__post_frontend",
    test_data_dir / "chstone_xilinx__post_frontend",
)

designs = (
    dataset_polybench.designs
    + dataset_machsuite.designs
    + dataset_chstone.designs
)

xilinx_aggregator = DataAggregatorXilinx()
data = xilinx_aggregator.gather_multiple_designs(designs, n_jobs=32)
xilinx_aggregator.aggregated_data_to_archive(data, Path("demos/demo_ml/data/data.zip"))
```

You can run the complete collection step via:

```bash
python demos/demo_ml/ml_agg.py
```

This produces `demos/demo_ml/data/data.zip`, which bundles the raw CSV export
and per-design report archives required downstream.

## Step 2: Enrich Features from HLS Reports

The preprocessing script expands the dataset by parsing expression, multiplexer,
and binding summaries from each design’s report bundle. It also filters out
rows missing runtime measurements.

```python
zip_fp = DATA_DIR / "data.zip"
assert zip_fp.exists()
data_all_zip_fp = "data_all.csv"

with zipfile.ZipFile(zip_fp, "r") as z, z.open(data_all_zip_fp) as f:
    data_all = pd.read_csv(f)
# ...
data_all = data_all[
    data_all["execution__VitisHLSSynthFlow"].notna()
    & data_all["execution__VitisHLSImplFlow"].notna()
    & data_all["execution__VitisHLSImplReportFlow"].notna()
]
# ...
print(data_all["synthesis__clock_period"])
```

```python
extra_data = []

for design_id in data_all["design_id"]:
    print(design_id)

    data = {}
    data["design_id"] = design_id

    with zipfile.ZipFile(zip_fp, "r") as z:
        artifacts_zip_fp = f"{design_id}/artifacts.zip"
        buf_artifacts = BytesIO(z.read(artifacts_zip_fp))
    # ...
    for rpt_file_name in report_files:
        with zipfile.ZipFile(buf_artifacts, "r") as z:
            with z.open(f"artifacts/report/{rpt_file_name}") as f:
                rpt_raw = f.read().decode("utf-8")

        block = get_report_table_block(rpt_raw, "Expression")
        if block is not None:
            table_df = parse_ascii_table(block)
            table_df["DSP"] = table_df["DSP"].astype(int)
            table_df["FF"] = table_df["FF"].astype(int)
            table_df["LUT"] = table_df["LUT"].astype(int)
            table_df["Bitwidth P0"] = table_df["Bitwidth P0"].astype(int)
            # ...
            df_expressions_all.append(table_df)

        block = get_report_table_block(rpt_raw, "Multiplexer")
        if block is not None:
            table_df = parse_ascii_table(block)
            table_df["LUT"] = table_df["LUT"].astype(int)
            table_df["Input Size"] = table_df["Input Size"].astype(int)
            table_df["Bits"] = table_df["Bits"].astype(int)
            table_df["Total Bits"] = table_df["Total Bits"].astype(int)
            df_mux_all.append(table_df)

    df_expressions = pd.concat(df_expressions_all)
    df_expressions = df_expressions.groupby("Operation").agg(
        {
            "Operation": "count",
            "LUT": "sum",
            "FF": "sum",
            "DSP": "sum",
        },
    )
    for op in ops_to_check_for:
        if op in df_expressions.index:
            expression_data[f"op_{op}_count"] = df_expressions.loc[op, "Operation"]
            expression_data[f"op_{op}_lut"] = df_expressions.loc[op, "LUT"]
            expression_data[f"op_{op}_ff"] = df_expressions.loc[op, "FF"]
            expression_data[f"op_{op}_dsp"] = df_expressions.loc[op, "DSP"]
        else:
            expression_data[f"op_{op}_count"] = 0
            expression_data[f"op_{op}_lut"] = 0
            expression_data[f"op_{op}_ff"] = 0
            expression_data[f"op_{op}_dsp"] = 0

    if df_mux_all != []:
        df_mux = pd.concat(df_mux_all)
        mux_data["mux_lut_sum"] = df_mux["LUT"].sum()
        mux_data["mux_input_size_sum"] = df_mux["Input Size"].sum()
        mux_data["mux_bits_sum"] = df_mux["Bits"].sum()
        mux_data["mux_total_bits_sum"] = df_mux["Total Bits"].sum()

    # ...
    df_bind = pd.DataFrame(
        {
            "optype": optypes,
            "dsp": bind_dsp,
            "latency": bind_latency,
        },
    )
    df_bind["dsp"] = df_bind["dsp"].astype(int)
    df_bind["latency"] = df_bind["latency"].astype(int)

    df_agg = df_bind.groupby("optype").agg(
        {
            "optype": "count",
            "dsp": "sum",
            "latency": "sum",
        },
    )

    for bind_op in bind_to_check_for:
        if bind_op in df_agg.index:
            data_bind[f"bind_{bind_op}_count"] = df_agg.loc[bind_op, "optype"]
            data_bind[f"bind_{bind_op}_dsp"] = df_agg.loc[bind_op, "dsp"]
            data_bind[f"bind_{bind_op}_latency"] = df_agg.loc[bind_op, "latency"]
        else:
            data_bind[f"bind_{bind_op}_count"] = 0
            data_bind[f"bind_{bind_op}_dsp"] = 0
            data_bind[f"bind_{bind_op}_latency"] = 0

    for key, value in expression_data.items():
        data[f"expression__{key}"] = value
    for key, value in mux_data.items():
        data[f"mux__{key}"] = value

    for key, value in data_bind.items():
        data[f"binding__{key}"] = value

    extra_data.append(data)
```

```bash
python demos/demo_ml/ml_pre.py
```

Key additions include operation-level counts and resource usage broken down by
operator type, multiplexer statistics, and bind-level latency summaries. The
augmented dataset is stored at `demos/demo_ml/data/data_all.csv` for reuse in
interactive notebooks or the modeling stage.

## Step 3: Train Multi-output QoR Predictors

Execute the modeling script to train hist-gradient boosting regressors that
predict LUT, FF, BRAM18, DSP utilization, and timing slack metrics from the
feature set created above.

```python
x = data_all[input_columns]
y = data_all[output_columns]
kf = KFold(n_splits=5, shuffle=True, random_state=42)
x_train, x_test, y_train, y_test, data_all_train, data_all_test = train_test_split(
    x,
    y,
    data_all,
    test_size=0.2,
    random_state=42,
)


reg = HistGradientBoostingRegressor(
    # verbose=11,
    max_iter=500,
    max_depth=None,
    l2_regularization=1.0,
    random_state=42,
    loss="squared_error",
)
mor = MultiOutputRegressor(reg)

mor_half = deepcopy(mor)


mor.fit(x_train, y_train)
print(mor.score(x_test, y_test))
# ...
```

```bash
python demos/demo_ml/ml_model.py
```

The script performs the following:

- Splits data into train/test sets and fits a `MultiOutputRegressor` with a
  `HistGradientBoostingRegressor` base estimator.
- Trains a reduced model on 25% of the training data to study data-efficiency.
- Benchmarks predictions against raw HLS estimates (when available).
- Generates a scatter-plot matrix comparing true versus predicted values and
  annotates each subplot with R² and relative absolute error (RAE) metrics.

```python
num_output_columns = len(output_columns)
n_rows = 2
n_cols = 3
fig, axs = plt.subplots(n_rows, n_cols, figsize=(8, 5))
axs = axs.flatten()
for col_idx, col in enumerate(output_columns):
    ax = axs[col_idx]

    true_train = y_train[col]
    true_test = y_test[col]
    pred_train = y_pred_train[:, col_idx]
    pred_test = y_pred_test[:, col_idx]

    pred_test_half = y_pred_test_half[:, col_idx]

    # compute the R2 for the test set
    r2_test = r2_score(true_test, pred_test)
    mae = np.mean(np.abs(true_test - pred_test))

    rae_test = np.sum(np.abs(true_test - pred_test)) / (
        np.sum(np.abs(true_test - true_test.mean())) + np.finfo(float).eps
    )

    rae_test_half = np.sum(np.abs(true_test - pred_test_half)) / (
        np.sum(np.abs(true_test - true_test.mean())) + np.finfo(float).eps
    )
    r2_test_half = r2_score(true_test, pred_test_half)

    marker_size = 20

    # ax.scatter(
    #     y_train[col],
    #     y_pred_train[:, col_idx],
    #     label="train",
    #     color=colors["train"],
    #     s=marker_size,
    #     zorder=10,
    #     alpha=0.8,
    # )

    # half
    ax.scatter(
        y_test[col],
        y_pred_test_half[:, col_idx],
        label="test half",
        color=colors["train"],
        s=marker_size,
        zorder=10,
        alpha=0.8,
    )

    ax.scatter(
        y_test[col],
        y_pred_test[:, col_idx],
        label="test",
        color=colors["test"],
        s=marker_size,
        zorder=11,
        alpha=0.8,
    )
    # ...

    if col in impl_to_hls_resource_map:
        hls_col = impl_to_hls_resource_map[col]
        hls_pred = data_all_test[hls_col]

        ax.scatter(y_test[col], hls_pred, label="HLS", marker="x", color=colors["hls"])
        rae_hls = np.sum(np.abs(true_test - hls_pred)) / (
            np.sum(np.abs(true_test - true_test.mean())) + np.finfo(float).eps
        )
        r2_hls = r2_score(true_test, hls_pred)

    ax.set_title(label_map[col])
    ax.set_xlabel("True Value")
    ax.set_ylabel("Predicted Value")

    # text_str = f"R2: {r2_test:.2f}\nML Full RAE: {rae_test:.2f}\nML Half RAE: {rae_test_half:.2f}"
    # text_str = ""
    # text_str += f"R2: {r2_test:.2f}\n"
    # text_str += f"ML Half RAE: {rae_test_half:.2f}\n"
    # text_str += f"ML Full RAE: {rae_test:.2f}\n"
    # if col in impl_to_hls_resource_map:
    #     text_str += f"HLS RAE: {rae_hls:.2f}"

    text_str = ""
    # make a latex string
    text_str += f"ML (25%): R2={r2_test_half:.2f}, RAE={rae_test_half:.2f}\n"
    text_str += f"ML (100%): R2={r2_test:.2f}, RAE={rae_test:.2f}"
    if col in impl_to_hls_resource_map:
        text_str += "\n"
        text_str += f"HLS: R2={r2_hls:.2f}, RAE={rae_hls:.2f}"

    ax.text(
        # place bottom right of the axes
        # 0.97,
        # 0.04,
        0.96,
        0.95,
        # 1.0,
        # 0.0,
        text_str,
        # place bottom right of the axes
        transform=ax.transAxes,
        verticalalignment="top",
        horizontalalignment="right",
        # text alignment right
        # transform=ax.transAxes,
        # verticalalignment="top",
        # weight="bold",
        # white background with some padding
        bbox={
            "facecolor": "white",
            "edgecolor": "black",
            "pad": 4,
            "alpha": 0.9,
            "linewidth": 0.0,
        },
        fontsize=7.75,
    ).set_zorder(100)

    # add grid in the background
    ax.grid(True, linestyle="--", alpha=0.5, zorder=-1)
# ...
```

Artifacts are written under `demos/demo_ml/figures/`, notably
`qor_scatter.png`, which visualizes the QoR prediction performance.

## Extending the Workflow

- Swap in alternative model families (e.g., random forests, neural networks) by
  modifying the estimator definition in `ml_model.py`.
- Experiment with feature selection or additional handcrafted features by
  editing `ml_pre.py`.
- Integrate the generated CSV into your broader analytics stack to track
  prediction accuracy as your dataset grows.

The demo offers a practical starting point for data-driven QoR estimation,
connecting HLSFactory’s dataset abstractions to scikit-learn pipelines. Once
you are comfortable with the defaults here, you can embed the pipeline into
automated regression tracking alongside the benchmarking flow described in
[](./tool_bench.md).
