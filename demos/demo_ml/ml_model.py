import re
import xml.etree.ElementTree as ET
import zipfile
from copy import deepcopy
from io import BytesIO
from pathlib import Path
from pprint import pp

import matplotlib
import numpy as np
import pandas as pd
from matplotlib import pyplot as plt
from matplotlib.axes import Axes
from sklearn.compose import ColumnTransformer
from sklearn.ensemble import (
    GradientBoostingRegressor,
    HistGradientBoostingRegressor,
    RandomForestRegressor,
)
from sklearn.impute import SimpleImputer
from sklearn.kernel_approximation import Nystroem
from sklearn.linear_model import LinearRegression
from sklearn.metrics import r2_score
from sklearn.model_selection import KFold, train_test_split
from sklearn.multioutput import MultiOutputRegressor
from sklearn.neural_network import MLPRegressor
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import RobustScaler, StandardScaler
from sklearn.svm import SVR

CURRENT_DIR = Path(__file__).parent
DATA_DIR = CURRENT_DIR / "data"
FIGURES_DIR = CURRENT_DIR / "figures"
FIGURES_DIR.mkdir(exist_ok=True)


data_all = pd.read_csv(DATA_DIR / "data_all.csv")

expression_cols = [col for col in data_all.columns if "expression__" in col]
mux_cols = [col for col in data_all.columns if "mux__" in col]
binding_cols = [col for col in data_all.columns if "binding__" in col]

input_columns = (
    [
        "synthesis__clock_period",
        "synthesis__latency_best_cycles",
        "synthesis__latency_best_seconds",
        "synthesis__latency_average_cycles",
        "synthesis__latency_average_seconds",
        "synthesis__latency_worst_cycles",
        "synthesis__latency_worst_seconds",
        "synthesis__resources_lut_used",
        "synthesis__resources_ff_used",
        "synthesis__resources_dsp_used",
        "synthesis__resources_bram_used",
        "synthesis__resources_uram_used",
    ]
    + expression_cols
    + mux_cols
    + binding_cols
)

output_columns = [
    "implementation__utilization__Total LUTs",
    # "implementation__utilization__Logic LUTs",
    # "implementation__utilization__LUTRAMs",
    # "implementation__utilization__SRLs",
    "implementation__utilization__FFs",
    # "implementation__utilization__RAMB36",
    "implementation__utilization__RAMB18",
    "implementation__utilization__DSP Blocks",
    "implementation__timing__wns",
    "implementation__timing__whs",
]

impl_to_hls_resource_map = {
    "implementation__utilization__Total LUTs": "synthesis__resources_lut_used",
    "implementation__utilization__FFs": "synthesis__resources_ff_used",
    "implementation__utilization__RAMB18": "synthesis__resources_bram_used",
    "implementation__utilization__DSP Blocks": "synthesis__resources_dsp_used",
}


data_reg = data_all[input_columns + output_columns]
# print any rows with NaN values
# show all cols
pd.set_option("display.max_columns", None)
print(data_all.columns)
print(data_all.head())


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


y_pred_train = mor.predict(x_train)
y_pred_test = mor.predict(x_test)

y_pred_train = np.asarray(y_pred_train)
y_pred_test = np.asarray(y_pred_test)


# x_train_half = x_train.iloc[: len(x_train) // 3]
# y_train_half = y_train.iloc[: len(y_train) // 3]
# data_all_half = data_all_train.iloc[: len(data_all_train) // 3]

# keep half the data
x_train_half = x_train.iloc[: len(x_train) // 4]
y_train_half = y_train.iloc[: len(y_train) // 4]
data_all_half = data_all_train.iloc[: len(data_all_train) // 4]

mor_half.fit(x_train_half, y_train_half)
print(mor_half.score(x_test, y_test))

y_pred_train_half = mor_half.predict(x_train_half)
y_pred_test_half = mor_half.predict(x_test)

y_pred_train_half = np.asarray(y_pred_train_half)
y_pred_test_half = np.asarray(y_pred_test_half)


label_map = {
    "implementation__utilization__Total LUTs": "LUTs",
    "implementation__utilization__FFs": "FFs",
    "implementation__utilization__RAMB18": "RAMB18s",
    "implementation__utilization__DSP Blocks": "DSP Blocks",
    "implementation__timing__wns": "Worst Negative Slack",
    "implementation__timing__whs": "Worst Hold Slack",
}

colors = {
    "train": "#0096c7",
    "test": "#ff9f1c",
    "hls": "#c1121f",
}

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

    max_val = max(y_train[col].max(), y_test[col].max())
    min_val = min(y_train[col].min(), y_test[col].min())
    ax.plot([min_val, max_val], [min_val, max_val], color="black", linestyle="--")
    ax.set_xlim(min_val, max_val)
    ax.set_ylim(min_val, max_val * 1.5)

    if max_val > 1000:
        # format with commas
        # {x:,.0f}
        # ax.get_xaxis().set_major_formatter(
        #     matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ",")),
        # )
        # ax.get_yaxis().set_major_formatter(
        #     matplotlib.ticker.FuncFormatter(lambda x, p: format(int(x), ",")),
        # )
        # builtin scientific notation
        # ax.get_xaxis().get_major_formatter().set_scientific(True)
        # ax.get_yaxis().get_major_formatter().set_scientific(True)

        ax.ticklabel_format(style="sci", axis="both", scilimits=(0, 0))

        ax.xaxis.set_major_locator(plt.MaxNLocator(5))
        ax.yaxis.set_major_locator(plt.MaxNLocator(5))

    if not min_val < 0:
        ax.set_xlim(0, max_val)
        ax.set_ylim(0, max_val * 1.5)

    # set tick font size
    ax.tick_params(axis="both", which="major", labelsize=8)

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
        bbox=dict(
            facecolor="white",
            edgecolor="black",
            pad=4,
            alpha=0.9,
            linewidth=0.0,
        ),
        fontsize=7.75,
    ).set_zorder(100)

    # add grid in the background
    ax.grid(True, linestyle="--", alpha=0.5, zorder=-1)


leg_artists = [
    plt.Rectangle((0, 0), 1, 1, fc=colors["train"], label="ML 25% Design Space"),
    plt.Rectangle((0, 0), 1, 1, fc=colors["test"], label="ML Full Design Space"),
    plt.Rectangle((0, 0), 1, 1, fc=colors["hls"], label="HLS Reported"),
]

# figure legend
fig.legend(
    handles=leg_artists,
    loc="lower center",
    ncol=3,
    columnspacing=1,
)

fig.suptitle("Xilinx Post-Implementation QoR Prediction", fontsize=16)

print("Saving figure...")

fig.tight_layout(
    h_pad=0.95,
    w_pad=0.95,
)
fig.subplots_adjust(top=0.87, bottom=0.16)
fig.savefig(FIGURES_DIR / "qor_scatter.png", dpi=300)
