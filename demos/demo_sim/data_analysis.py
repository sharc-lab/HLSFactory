import json
from pathlib import Path

import pandas as pd

from hlsfactory.utils import DirSource, get_work_dir

DIR_CURRENT_SCRIPT = Path(__file__).parent

DATA_DIR = DIR_CURRENT_SCRIPT / "data"
DATA_DIR.mkdir(exist_ok=True)

WORK_DIR_TOP = get_work_dir(dir_source=DirSource.ENVFILE)
WORK_DIR = WORK_DIR_TOP / "demo_sim"

DATASET_DIR = WORK_DIR / "stream_hls"


def pct_error(value: float, reference: float) -> float:
    return (value - reference) / reference * 100.0


rows = []
for design_dir in sorted(DATASET_DIR.glob("*")):
    if not design_dir.is_dir():
        continue

    data_cosim_fp = design_dir / "data_cosim.json"
    data_hls_fp = design_dir / "data_hls.json"
    data_lightningsim_fp = design_dir / "data_lightningsim.json"

    # Co-sim latency is the ground truth this case study compares against;
    # without it there's nothing to compute an error relative to.
    if not data_cosim_fp.exists() or not data_hls_fp.exists():
        continue

    data_cosim = json.loads(data_cosim_fp.read_text())
    data_hls = json.loads(data_hls_fp.read_text())

    cosim_latency = data_cosim["latency_avg_cycles"]
    hls_estimated_latency = data_hls["latency_average_cycles"]

    if cosim_latency is None or hls_estimated_latency is None:
        continue

    lightningsim_latency = None
    if data_lightningsim_fp.exists():
        data_lightningsim = json.loads(data_lightningsim_fp.read_text())
        lightningsim_latency = data_lightningsim["latency"]

    rows.append(
        {
            "design": design_dir.name,
            "cosim_latency": cosim_latency,
            "hls_estimated_latency": hls_estimated_latency,
            "hls_estimated_pct_error": pct_error(hls_estimated_latency, cosim_latency),
            "lightningsim_latency": lightningsim_latency,
            "lightningsim_pct_error": (
                pct_error(lightningsim_latency, cosim_latency)
                if lightningsim_latency is not None
                else None
            ),
        },
    )

df_data = pd.DataFrame(rows)
df_data.to_csv(DATA_DIR / "sim_latency_comparison_raw.csv", index=False)


def format_matches_cosim(value: float | None, cosim_value: float) -> bool:
    return value is not None and round(value) == round(cosim_value)


def format_value_with_pct_error(value: float | None, cosim_value: float) -> str:
    if value is None:
        return "N/A"
    if format_matches_cosim(value, cosim_value):
        return r"\checkmark"
    error = pct_error(value, cosim_value)
    sign = "+" if error >= 0 else ""
    return f"{value:.0f} / {sign}{error:.2f}\\%"


table_rows = []
for row in rows:
    table_rows.append(
        {
            "Design": row["design"],
            "Co-Sim": f"{row['cosim_latency']:.0f}",
            "Estimated from C-Synth": format_value_with_pct_error(
                row["hls_estimated_latency"],
                row["cosim_latency"],
            ),
            "LightningSim": format_value_with_pct_error(
                row["lightningsim_latency"],
                row["cosim_latency"],
            ),
        },
    )

df_table = pd.DataFrame(table_rows)

# Mean Absolute Percentage Error over designs with a valid, non-deadlocked result.
mape_hls = df_data["hls_estimated_pct_error"].abs().mean()
mape_lightningsim = df_data["lightningsim_pct_error"].dropna().abs().mean()

df_table = pd.concat(
    [
        df_table,
        pd.DataFrame(
            [
                {
                    "Design": "Mean Absolute Percentage Error",
                    "Co-Sim": "",
                    "Estimated from C-Synth": f"{mape_hls:.2f}\\%",
                    "LightningSim": f"{mape_lightningsim:.2f}\\%",
                },
            ],
        ),
    ],
    ignore_index=True,
)

df_table.to_csv(DATA_DIR / "sim_latency_comparison.csv", index=False)
df_table.to_latex(
    DATA_DIR / "sim_latency_comparison.txt",
    index=False,
)

print(df_table.to_string(index=False))
print(f"\nMean Absolute Percentage Error (Estimated from C-Synth): {mape_hls:.2f}%")
print(f"Mean Absolute Percentage Error (LightningSim): {mape_lightningsim:.2f}%")


### Runtime Comparison Table ###
# Compares how long each flow took to run per design, using the per-flow
# `dt` entries that log_execution_time_to_file writes to
# execution_time_data.json.

runtime_rows = []
for design_dir in sorted(DATASET_DIR.glob("*")):
    if not design_dir.is_dir():
        continue

    execution_time_fp = design_dir / "execution_time_data.json"
    if not execution_time_fp.exists():
        continue

    execution_time_data = json.loads(execution_time_fp.read_text())

    csynth_runtime_s = execution_time_data.get("VitisHLSSynthFlow", {}).get("dt")
    cosim_runtime_s = execution_time_data.get("VitisHLSCosimFlow", {}).get("dt")
    lightningsim_runtime_s = execution_time_data.get("LightningSimFlow", {}).get("dt")

    if csynth_runtime_s is None and cosim_runtime_s is None and lightningsim_runtime_s is None:
        continue

    runtime_rows.append(
        {
            "design": design_dir.name,
            "csynth_runtime_s": csynth_runtime_s,
            "cosim_runtime_s": cosim_runtime_s,
            "lightningsim_runtime_s": lightningsim_runtime_s,
            "cosim_speedup_vs_csynth": (
                cosim_runtime_s / csynth_runtime_s
                if cosim_runtime_s is not None and csynth_runtime_s
                else None
            ),
            "cosim_speedup_vs_lightningsim": (
                cosim_runtime_s / lightningsim_runtime_s
                if cosim_runtime_s is not None and lightningsim_runtime_s
                else None
            ),
        },
    )

df_runtime = pd.DataFrame(runtime_rows)
df_runtime.to_csv(DATA_DIR / "sim_runtime_comparison_raw.csv", index=False)


def format_runtime(value: float | None) -> str:
    return "N/A" if value is None else f"{value:.2f}"


def format_speedup(value: float | None) -> str:
    return "N/A" if value is None else f"{value:.1f}x"


runtime_table_rows = [
    {
        "Design": row["design"],
        "C-Synth (s)": format_runtime(row["csynth_runtime_s"]),
        "Co-Sim (s)": format_runtime(row["cosim_runtime_s"]),
        "LightningSim (s)": format_runtime(row["lightningsim_runtime_s"]),
        "Co-Sim / C-Synth": format_speedup(row["cosim_speedup_vs_csynth"]),
        "Co-Sim / LightningSim": format_speedup(row["cosim_speedup_vs_lightningsim"]),
    }
    for row in runtime_rows
]

df_runtime_table = pd.DataFrame(runtime_table_rows)

mean_cosim_speedup_vs_csynth = df_runtime["cosim_speedup_vs_csynth"].dropna().mean()
mean_cosim_speedup_vs_lightningsim = (
    df_runtime["cosim_speedup_vs_lightningsim"].dropna().mean()
)

df_runtime_table = pd.concat(
    [
        df_runtime_table,
        pd.DataFrame(
            [
                {
                    "Design": "Mean",
                    "C-Synth (s)": "",
                    "Co-Sim (s)": "",
                    "LightningSim (s)": "",
                    "Co-Sim / C-Synth": format_speedup(mean_cosim_speedup_vs_csynth),
                    "Co-Sim / LightningSim": format_speedup(
                        mean_cosim_speedup_vs_lightningsim,
                    ),
                },
            ],
        ),
    ],
    ignore_index=True,
)

df_runtime_table.to_csv(DATA_DIR / "sim_runtime_comparison.csv", index=False)
df_runtime_table.to_latex(
    DATA_DIR / "sim_runtime_comparison.txt",
    index=False,
)

print()
print(df_runtime_table.to_string(index=False))
