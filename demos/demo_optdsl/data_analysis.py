import json
from pathlib import Path
from pprint import pp

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
from matplotlib.lines import Line2D
from matplotlib.ticker import PercentFormatter

DIR_CURRENT = Path(__file__).parent

DIR_EVAL_WORK_DIR = DIR_CURRENT / "eval_work_dir"

label_map = {
    "openai/gpt-oss-20b": "gpt-oss-20B",
    "openai/gpt-oss-120b": "gpt-oss-120B",
}


def gather_data(eval_work_dir: Path) -> pd.DataFrame:
    single_eval_data_files = sorted(eval_work_dir.glob("**/eval_data_single.json"))

    data_for_table = []
    for single_eval_data_file in single_eval_data_files:
        design_data = json.loads(single_eval_data_file.read_text())

        design_name = design_data["design"]
        model = design_data["model"]
        k_index = design_data["k_index"]

        can_parse = bool(design_data.get("is_valid_opt_dsl", False))

        data_for_table.append(
            {
                "design_name": design_name,
                "design_idx": k_index,
                "model": model,
                "can_parse": can_parse,
            }
        )

    columns = ["design_name", "design_idx", "model", "can_parse"]
    df = pd.DataFrame(data_for_table, columns=columns)

    return df


df_all = gather_data(DIR_EVAL_WORK_DIR)
pp(df_all)


def pass_at_k(n, c, k):
    """
    :param n: total number of samples
    :param c: number of correct samples
    :param k: k in pass@$k$
    """
    if n - c < k:
        return 1.0
    return 1.0 - np.prod(1.0 - k / np.arange(n - c + 1, n + 1))


# for each (design, model) case compute the pass@1 value, using the pass_at_k function
df_summary = (
    df_all.groupby(["design_name", "model"])
    .agg(
        total_samples=("can_parse", "count"),
        correct_samples=("can_parse", "sum"),
    )
    .reset_index()
)

# now also make a new dataframe with a row for each (design, model, k) case for k in [1-5]
k_values = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
df_summary_k = pd.DataFrame(columns=["design_name", "model", "k", "pass_at_k"])
for k in k_values:
    df_k = df_summary.copy()
    df_k["k"] = k
    df_k["pass_at_k"] = df_k.apply(
        lambda row: pass_at_k(row["total_samples"], row["correct_samples"], k=k),
        axis=1,
    )
    df_summary_k = pd.concat([df_summary_k, df_k], ignore_index=True)
pp(df_summary_k)

# average pass@k for each model
df_summary_k_avg = (
    df_summary_k.groupby(["model", "k"])
    .agg(avg_pass_at_k=("pass_at_k", "mean"))
    .reset_index()
)
pp(df_summary_k_avg)

# now make a figure with a single axs

color_map = {
    "openai/gpt-oss-20b": "#4ea8de",
    "openai/gpt-oss-120b": "#38b000",
}
zorder_map = {
    "openai/gpt-oss-20b": 10,
    "openai/gpt-oss-120b": 20,
}


fig, ax = plt.subplots(figsize=(4, 4))
ax.grid(True, zorder=-100)
ax.set_axisbelow(True)


for model in df_summary_k_avg["model"].unique():
    df_plot = df_summary_k_avg[df_summary_k_avg["model"] == model]
    ax.plot(
        df_plot["k"],
        df_plot["avg_pass_at_k"],
        marker="o",
        label=label_map[model],
        zorder=zorder_map[model],
        color=color_map[model],
    )

# make a pass at k plot with lines for each (design, model) case
for model in df_summary_k["model"].unique():
    df_plot = df_summary_k[df_summary_k["model"] == model]
    for design_name in df_plot["design_name"].unique():
        df_plot_design = df_plot[df_plot["design_name"] == design_name]
        ax.plot(
            df_plot_design["k"],
            df_plot_design["pass_at_k"],
            marker="o",
            label=f"{label_map[model]} - {design_name}",
            alpha=0.2,
            color="gray",
            linestyle="--",
            zorder=2,
        )


ax.set_xlabel("$k$ (number of LLM samples)")
ax.set_ylabel("pass@$k$\n(at least one valid spec)")
ax.set_title("Average Pass@$k$ vs $k$ for\nLLM Generation of OptDSL Specs")
ax.set_xticks(k_values)
ax.set_ylim(0.7, 1.05)
ax.set_yticks(np.arange(0.7, 1.00, 0.05))

ax.yaxis.set_major_formatter(PercentFormatter(1, decimals=0))

label_artists = [
    # add three fake artists for the legend that line up with three labels, one for 20b avg, one for 120b avg, one for designs
    Line2D(
        [0],
        [0],
        color=color_map["openai/gpt-oss-20b"],
        marker="o",
        label="Avg. Pass@$k$ - gpt-oss-20B",
    ),
    Line2D(
        [0],
        [0],
        color=color_map["openai/gpt-oss-120b"],
        marker="o",
        label="Avg. Pass@$k$ - gpt-oss-120B ",
    ),
    Line2D(
        [0],
        [0],
        color="gray",
        marker="o",
        linestyle="--",
        alpha=0.5,
        label="Pass@$k$ - Individual Designs Cases",
    ),
]
ax.legend(handles=label_artists, loc="lower right", fontsize=8)

plt.tight_layout()
plt.savefig(DIR_CURRENT / "optdsl__pass_at_k_plot.png", dpi=300)
# plt.close()

# make a latex table with the pass@1 and pass@5 values for each (design, model) case
# create a pivot with MultiIndex columns (model, k) so we can use multicolumn in LaTeX
df_table = df_summary_k[df_summary_k["k"].isin([1, 5])]
df_pivot = df_table.pivot_table(
    index="design_name",
    columns=["model", "k"],
    values="pass_at_k",
    aggfunc="mean",
)

# ensure consistent column order (models in label_map order, k in [1,5])
model_order = list(label_map.keys())
k_order = [1, 5]
cols = pd.MultiIndex.from_product([model_order, k_order], names=["model", "k"])
df_pivot = df_pivot.reindex(columns=cols)

# replace model level with human-friendly labels
df_pivot.rename(columns=label_map, level=0, inplace=True)

# format values as LaTeX-ready percentage strings (use \% and disable escaping in to_latex)
df_formatted = df_pivot.apply(
    lambda col: col.map(lambda x: "" if pd.isna(x) else f"{x * 100:.1f}\\%")
)

df_formatted.to_latex(
    DIR_CURRENT / "optdsl__pass_at_k_table.tex",
    index=True,
    caption="Pass@1 and Pass@5 for each design and model",
    label="tab:pass_at_k",
    # booktabs=True,
    multicolumn=True,
    multicolumn_format="c",
    escape=False,
)
