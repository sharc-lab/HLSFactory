import json
from pathlib import Path

import pandas as pd
from scipy.stats import gmean, wilcoxon

from hlsfactory.utils import DirSource, get_work_dir

DIR_CURRENT_SCRIPT = Path(__file__).parent

FIGURES_DIR = DIR_CURRENT_SCRIPT / "figures"
FIGURES_DIR.mkdir(exist_ok=True)

DATA_DIR = DIR_CURRENT_SCRIPT / "data"
DATA_DIR.mkdir(exist_ok=True)


WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_regression_testing"

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

df_data = []
for design_data_single in design_data:
    design_id = design_data_single["design_id"]
    design_dir = design_data_single["design_dir"]
    vitis_version = design_data_single["version"]

    data_design_fp = design_dir / "data_design.json"
    data_hls_fp = design_dir / "data_hls.json"
    data_execution_time_fp = design_dir / "execution_time_data.json"

    if (
        not data_design_fp.exists()
        or not data_hls_fp.exists()
        or not data_execution_time_fp.exists()
    ):
        continue

    data_hls = json.loads(data_hls_fp.read_text())
    data_design = json.loads(data_design_fp.read_text())
    data_execution_time = json.loads(data_execution_time_fp.read_text())
    data_vitis_hls_execution_time = {
        "vitis_hls_dt": data_execution_time["VitisHLSSynthFlow"]["dt"],
    }

    ratio_data = {
        "design_id": design_id,
        "vitis_version": vitis_version,
        **data_design,
        **data_hls,
        **data_vitis_hls_execution_time,
    }
    df_data.append(ratio_data)

df_design_data = pd.DataFrame(df_data)

df_design_data = df_design_data[
    df_design_data["design_id"].map(df_design_data["design_id"].value_counts()) == 2
]
df_design_data = df_design_data.sort_values(by=["design_id", "vitis_version"])

version_0 = "2021_1"
version_1 = "2023_1"

df_version_0 = df_design_data[df_design_data["vitis_version"] == version_0].sort_values(
    by="design_id",
)
df_version_1 = df_design_data[df_design_data["vitis_version"] == version_1].sort_values(
    by="design_id",
)

columns_to_compute_ratios = [
    "vitis_hls_dt",
    "latency_best_cycles",
    "latency_average_cycles",
    "latency_worst_cycles",
    "resources_lut_used",
    "resources_ff_used",
]

df_ratio = df_version_1[columns_to_compute_ratios].reset_index(
    drop=True,
) / df_version_0[columns_to_compute_ratios].reset_index(drop=True)
ratio_data = {}
for column in columns_to_compute_ratios:
    data_column = df_ratio[column]
    data_column = data_column.dropna()
    ratio_average = gmean(data_column)
    ratio_percent_change = (ratio_average - 1) * 100
    ratio_median = data_column.median()
    ratio_median_percent_change = (ratio_median - 1) * 100
    ratio_data[column] = {
        "ratio_average": ratio_average,
        "ratio_average_percent_change": ratio_percent_change,
        "ratio_median": ratio_median,
        "ratio_median_percent_change": ratio_median_percent_change,
    }

median_data = {}
for column in columns_to_compute_ratios:
    data_column_version_0 = df_version_0[column].reset_index(drop=True)
    data_column_version_1 = df_version_1[column].reset_index(drop=True)
    median_data[column] = {
        "median_version_0": data_column_version_0.median(),
        "median_version_1": data_column_version_1.median(),
    }

test_data = {}
for column in columns_to_compute_ratios:
    data_column_version_0 = df_version_0[column].reset_index(drop=True)
    data_column_version_1 = df_version_1[column].reset_index(drop=True)

    df_combined = pd.DataFrame(
        {
            "version_0": data_column_version_0,
            "version_1": data_column_version_1,
        },
    )

    df_combined = df_combined.dropna()
    data_column_version_0 = df_combined["version_0"].to_numpy()
    data_column_version_1 = df_combined["version_1"].to_numpy()

    n_samples = len(data_column_version_0)

    _, p_less = wilcoxon(
        data_column_version_0,
        data_column_version_1,
        alternative="less",
    )

    _, p_greater = wilcoxon(
        data_column_version_0,
        data_column_version_1,
        alternative="greater",
    )

    _, p_two_sided = wilcoxon(
        data_column_version_0,
        data_column_version_1,
        alternative="two-sided",
    )

    test_data[column] = {
        "p_less": p_less,
        "p_greater": p_greater,
        "p_two_sided": p_two_sided,
        "n_samples": n_samples,
    }

data_table = {
    k: {"ratio": ratio_data[k], "test": test_data[k], "median": median_data[k]}
    for k in columns_to_compute_ratios
}
data_table_flat = [
    {
        "feature": k,
        **v["ratio"],
        **v["median"],
        **v["test"],
    }
    for k, v in data_table.items()
]

df_data_table = pd.DataFrame(data_table_flat)
df_data_table.to_csv(DATA_DIR / "regression_testing_stats.csv", index=False)
df_data_table.to_latex(
    DATA_DIR / "regression_testing_stats.txt",
    index=False,
)
