import zipfile
from pathlib import Path

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


zip_fp = DATA_DIR / "data.zip"
assert zip_fp.exists()
data_all_zip_fp = "data_all.csv"


with zipfile.ZipFile(zip_fp, "r") as z, z.open(data_all_zip_fp) as f:
    data_all = pd.read_csv(f)
print(data_all.columns)
print(data_all.head())

# filter rows where execution__VitisHLSSynthFlow or execution__VitisHLSImplFlow or execution__VitisHLSImplReportFlow is NaN
data_all = data_all[
    data_all["execution__VitisHLSSynthFlow"].notna()
    & data_all["execution__VitisHLSImplFlow"].notna()
    & data_all["execution__VitisHLSImplReportFlow"].notna()
]

print(data_all.shape)


input_columns = [
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

output_columns = [
    "implementation__utilization__Total LUTs",
    "implementation__utilization__Logic LUTs",
    "implementation__utilization__LUTRAMs",
    "implementation__utilization__SRLs",
    "implementation__utilization__FFs",
    "implementation__utilization__RAMB36",
    "implementation__utilization__RAMB18",
    "implementation__utilization__URAM",
    "implementation__utilization__DSP Blocks",
    "implementation__timing__wns",
    "implementation__timing__tns",
    "implementation__timing__whs",
    "implementation__timing__ths",
    "implementation__timing__wpws",
    "implementation__timing__tpws",
]


data_reg = data_all[input_columns + output_columns]
# print any rows with NaN values
# show all cols
pd.set_option("display.max_columns", None)
print(data_all.columns)
print(data_all.head())

x = data_all[input_columns]
y = data_all[output_columns]

kf = KFold(n_splits=5, shuffle=True, random_state=42)
# reg = HistGradientBoostingRegressor()
# mor = MultiOutputRegressor(reg)
# build a pipeline with simple rbs nystroem kernel
mor = Pipeline(
    [
        # (
        #     "preprocess",
        #     SimpleImputer(strategy="median", add_indicator=True),
        # ),
        # ("scale", RobustScaler()),
        ("reg", MultiOutputRegressor(HistGradientBoostingRegressor(random_state=42))),
    ],
)


for train_index, test_index in kf.split(x):
    x_train, x_test = x.iloc[train_index], x.iloc[test_index]
    y_train, y_test = y.iloc[train_index], y.iloc[test_index]
    mor.fit(x_train, y_train)
    print(mor.score(x_test, y_test))

# train test split to train final model
x_train, x_test, y_train, y_test = train_test_split(
    x,
    y,
    test_size=0.2,
    random_state=42,
)
mor.fit(x_train, y_train)
print(mor.score(x_test, y_test))


y_pred_train = mor.predict(x_train)
y_pred_test = mor.predict(x_test)

y_pred_train = np.asarray(y_pred_train)
y_pred_test = np.asarray(y_pred_test)

num_output_columns = len(output_columns)
n_rows = 3
n_cols = 6
fig, axs = plt.subplots(n_rows, n_cols, figsize=(15, 10))
axs = axs.flatten()
for col_idx, col in enumerate(output_columns):
    ax = axs[col_idx]

    true_train = y_train[col]
    true_test = y_test[col]
    pred_train = y_pred_train[:, col_idx]
    pred_test = y_pred_test[:, col_idx]

    # compute the R2 for the test set
    r2_test = r2_score(true_test, pred_test)
    mae = np.mean(np.abs(true_test - pred_test))

    ax.scatter(y_train[col], y_pred_train[:, col_idx], label="train")
    ax.scatter(y_test[col], y_pred_test[:, col_idx], label="test")

    max_val = max(y_train[col].max(), y_test[col].max())
    min_val = min(y_train[col].min(), y_test[col].min())
    ax.plot([min_val, max_val], [min_val, max_val], color="black", linestyle="--")
    ax.set_xlim(min_val, max_val)
    ax.set_ylim(min_val, max_val)

    ax.set_title(col.replace("implementation__", ""))
    ax.set_xlabel("True")
    ax.set_ylabel("Predicted")
    ax.legend(
        loc="lower right",
        fontsize="small",
    )

    ax.text(
        # place bottom right of the axes
        0.05,
        0.95,
        f"R2: {r2_test:.2f}\nMAE: {mae:.2f}",
        transform=ax.transAxes,
        verticalalignment="top",
    )
fig.tight_layout()
fig.savefig(FIGURES_DIR / "scatter.png", dpi=300)
