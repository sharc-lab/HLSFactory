import json
from pathlib import Path
from pprint import pp
from zipfile import ZipFile

import matplotlib.pyplot as plt
import pandas as pd
from matplotlib.ticker import FuncFormatter
from sklearn.compose import ColumnTransformer
from sklearn.decomposition import PCA
from sklearn.kernel_approximation import Nystroem
from sklearn.manifold import TSNE
from sklearn.pipeline import Pipeline
from sklearn.preprocessing import StandardScaler
from umap import UMAP

DIR_CURRENT = Path(__file__).parent
FP_DATA = DIR_CURRENT / "extracted_trace_run32hours.zip"

DIR_FIGURES = DIR_CURRENT / "figures"
DIR_FIGURES.mkdir(exist_ok=True)


def load_data(fp_data: Path) -> tuple[dict[str, pd.DataFrame], dict[str, pd.DataFrame]]:
    df_map = {}
    df_map_baseline = {}
    with ZipFile(fp_data, "r") as zip_file:
        for fp in zip_file.namelist():
            # make sure it's a json file
            if not fp.endswith(".json"):
                continue
            if fp.startswith("__MACOSX"):
                continue
            with zip_file.open(fp) as json_file:
                data = json.load(json_file)
                records = []
                for design_name, result in data.items():
                    perf = result["perf"]
                    valid = result["valid"]
                    point = result["point"]
                    res_util = result["res_util"]
                    record = {
                        "design_name": design_name,
                        "perf": perf,
                        "valid": valid,
                        **res_util,
                        **point,
                    }
                    records.append(record)
                df = pd.DataFrame.from_records(records)
                design_name = Path(fp).stem
                df_map[design_name] = df

                # make a df_baseline with the first entry in the records being the only row
                first_design_name, first_result = list(data.items())[0]
                first_record = {
                    "design_name": first_design_name,
                    "perf": first_result["perf"],
                    "valid": first_result["valid"],
                    **first_result["res_util"],
                    **first_result["point"],
                }
                df_baseline = pd.DataFrame.from_records([first_record])
                df_map_baseline[design_name] = df_baseline
    return df_map, df_map_baseline


if __name__ == "__main__":
    df_map, df_map_baseline = load_data(FP_DATA)
    for design_name, df in df_map.items():
        print(f"Design: {design_name}, DataFrame shape: {df.shape}")
    for design_name, df in df_map_baseline.items():
        print(f"Baseline Design: {design_name}, DataFrame shape: {df.shape}")

    PLOT_PCA = False
    PLOT_PARETO = True
    PLOT_BEST_IMPROVEMENT = False

    for design_name, df in sorted(df_map.items(), key=lambda x: x[0]):
        if PLOT_PCA:
            # PCA plot
            features = [
                col for col in df.columns if col.startswith(("__PARA__", "__PIPE__"))
            ]
            if not features:
                continue
            x = df[features].copy()
            for col in x.columns:
                if col.startswith("__PIPE__"):
                    x[col] = x[col].replace("", "none")
            x = pd.get_dummies(
                x, columns=[col for col in x.columns if col.startswith("__PIPE__")]
            )
            y = df["valid"]
            if len(x) == 0 or len(y) == 0:
                continue
            x_scaled = StandardScaler().fit_transform(x)
            # pca = PCA(n_components=2)
            # pca = TSNE(
            #     n_components=2,
            #     random_state=42,
            #     init="pca",
            #     learning_rate="auto",
            #     perplexity=10,
            # )
            #         (
            #     "tansform",
            #     UMAP(n_components=2, min_dist=0.5, n_neighbors=32, n_jobs=16, verbose=True),
            # ),
            pca = UMAP(
                n_components=2, min_dist=0.5, n_neighbors=32, n_jobs=32, verbose=True
            )

            principal_components = pca.fit_transform(x_scaled)
            df_pca = pd.DataFrame(
                data=principal_components,
                columns=["principal_component_1", "principal_component_2"],
            )
            df_pca["valid"] = y.to_numpy()

            color_map_valid = {True: "green", False: "red"}
            plt.figure(figsize=(4, 4))
            plt.scatter(
                df_pca["principal_component_1"],
                df_pca["principal_component_2"],
                c=df_pca["valid"].map(color_map_valid),
                alpha=0.5,
            )
            # dont show axis ticks
            plt.xticks([])
            plt.yticks([])
            # make sure the aspect ratio is square 1x1
            plt.gca().set_aspect("equal", adjustable="box")
            plt.xlabel("Projection $x_0$")
            plt.ylabel("Projection $x_1$")
            plt.title(f"Design Space of Searched DSE\nConfigurations for {design_name}")
            # plt.tight_layout()
            plt.savefig(DIR_FIGURES / f"dim_reduction__{design_name}.png", dpi=300)
            plt.close()

        if PLOT_PARETO:
            # Pareto plot for valid designs
            df_valid: pd.DataFrame = df[df["valid"]]
            if df_valid.empty:
                continue
            df_valid = df_valid.copy()
            df_valid["used_LUT"] = df_valid["total-LUT"]
            df_valid_sorted = df_valid.sort_values("used_LUT")

            df_valid["area_latency_product"] = df_valid["used_LUT"] * df_valid["perf"]
            best_alp_row = df_valid.loc[df_valid["area_latency_product"].idxmin()]
            print(
                f"Design: {design_name}, Best Area-Latency Product: {best_alp_row['area_latency_product']}, LUTs: {best_alp_row['used_LUT']}, Latency: {best_alp_row['perf']}"
            )

            x_pareto = df_valid_sorted["used_LUT"]
            y_pareto = df_valid_sorted["perf"]
            y_front = y_pareto.copy()
            min_y = float("inf")
            for i in range(len(y_front)):
                min_y = min(min_y, y_front.iloc[i])
                y_front.iloc[i] = min_y

            plt.figure(figsize=(4, 4))

            # create gird for major and minor ticks
            plt.grid(
                axis="both", which="major", linestyle="-", linewidth=1.0, alpha=0.7
            )
            plt.grid(
                axis="both", which="minor", linestyle="--", linewidth=0.5, alpha=0.4
            )
            plt.gca().set_axisbelow(True)

            plt.plot(x_pareto, y_front, "r-", linewidth=1, label="Pareto front")
            plt.scatter(
                df_valid["used_LUT"],
                df_valid["perf"],
                alpha=0.5,
                label="Valid designs",
                s=10,
            )

            # Mark the design with the lowest area-latency product
            best_x = int(best_alp_row["used_LUT"])
            best_y = int(best_alp_row["perf"])
            plt.scatter(
                [best_x],
                [best_y],
                color="gold",
                edgecolors="black",
                s=180,
                marker="*",
                label="Best Area-Latency Product",
                zorder=5,
            )
            # annotate the best point with ALP, LUT and latency
            # Compute formatted strings for LUTs and ALP
            lut_str = f"{int(best_x):.2e}"
            if "e" in lut_str:
                base, exp_part = lut_str.split("e")
                exp = int(exp_part)
                lut_str = f"{base} x 10^{exp}"

            latency_str = f"{int(best_y):.2e}"
            if "e" in latency_str:
                base, exp_part = latency_str.split("e")
                exp = int(exp_part)
                latency_str = f"{base} x 10^{exp}"

            alp_str = f"{int(best_alp_row['area_latency_product']):.2e}"
            if "e" in alp_str:
                base, exp_part = alp_str.split("e")
                exp = int(exp_part)
                alp_str = f"{base} x 10^{exp}"

            plt.annotate(
                f"LUTs: {lut_str}\nLat.: {latency_str}\nALP: {alp_str}",
                xy=(best_x, best_y),
                xytext=(10, -30),
                textcoords="offset points",
                fontsize=8,
                bbox=dict(boxstyle="round,pad=0.3", fc="white", alpha=0.8),
                # arrowprops=dict(arrowstyle="->", connectionstyle="arc3,rad=0"),
            )
            # make log log
            plt.xscale("log")
            plt.yscale("log")

            plt.xlabel("LUTs")
            plt.ylabel("Latency (Clock Cycles)")
            plt.title(f"Pareto Plot for {design_name}")
            plt.legend(fontsize=8)
            plt.tight_layout()
            plt.savefig(DIR_FIGURES / f"pareto__{design_name}.png", dpi=300)
            plt.close()

    if PLOT_BEST_IMPROVEMENT:
        # Plot best improvement over baseline latency for each design
        improvements_latency = {}
        improvements_lut = {}
        improvements_alp = {}

        for design_name, df in df_map.items():
            df_baseline = df_map_baseline[design_name]
            baseline_latency = df_baseline["perf"].iloc[0]
            baseline_lut = df_baseline["total-LUT"].iloc[0]
            baseline_alp = baseline_latency * baseline_lut

            df_valid = df[df["valid"]]
            if df_valid.empty:
                continue

            best_latency = df_valid["perf"].min()
            best_lut = df_valid["total-LUT"].min()
            best_alp = (df_valid["perf"] * df_valid["total-LUT"]).min()

            improvements_latency[design_name] = baseline_latency / best_latency
            improvements_lut[design_name] = baseline_lut / best_lut
            improvements_alp[design_name] = baseline_alp / best_alp

        pp(improvements_alp)

        data_alp_sorted = sorted(
            improvements_alp.items(), key=lambda x: x[1], reverse=False
        )

        data_latency_sorted = sorted(
            improvements_latency.items(),
            key=lambda x: [item[0] for item in data_alp_sorted].index(x[0]),
            reverse=False,
        )

        import seaborn as sns

        fig, axs = plt.subplots(2, 1, figsize=(12, 6))

        TITLE_FONT_SIZE = 16
        LABEL_FONT_SIZE = 14

        axs[0].grid(axis="y", which="major", linestyle="-", linewidth=1.0, alpha=1.0)
        axs[0].grid(axis="y", which="minor", linestyle="--", linewidth=0.5, alpha=0.7)
        axs[0].set_axisbelow(True)

        sns.barplot(
            # x=list(improvements_latency.keys()),
            x=[item[0] for item in data_latency_sorted],
            # y=list(improvements_latency.values()),
            y=[item[1] for item in data_latency_sorted],
            ax=axs[0],
            log_scale=(False, True),
            # hue="#ff9b54",
        )
        # set all the bar colors
        for i, patch in enumerate(axs[0].patches):
            patch.set_facecolor("#ff7f51")

        # for each bar, place the speedup value right above it
        # for i, patch in enumerate(axs[0].patches):
        #     height = patch.get_height()
        #     axs[0].text(
        #         patch.get_x() + patch.get_width() / 2,
        #         height + 1.0001 * height,
        #         f"{height:.1f}x",
        #         ha="center",
        #         va="bottom",
        #         fontsize=6,
        #         # put in a white box, no padding, no edge, rectangular
        #         bbox=dict(boxstyle="square,pad=0.0", fc="white", ec="none", alpha=0.8),
        #     )

        axs[0].set_yscale("log")
        axs[0].set_title("Latency Improvement over Baseline", fontsize=TITLE_FONT_SIZE)
        axs[0].set_ylabel("Improvement Ratio", fontsize=LABEL_FONT_SIZE)
        # axs[0].set_xlabel("Design Name")
        # axs[0].tick_params(axis="x", rotation=45, ha="right")
        axs[0].tick_params(axis="x", rotation=45)
        for label in axs[0].get_xticklabels():
            label.set_horizontalalignment("right")
            # format yticks with "x" at thhe end of currenbt tick
        # labels = [f"{tick.get_text()}x" for tick in axs[1].get_yticklabels()]
        # axs[0].set_yticklabels(labels)
        axs[0].yaxis.set_major_formatter(FuncFormatter(lambda y, _: f"{int(y)}x"))
        # set y lim to be 1050
        axs[0].set_ylim(top=1900)

        # sns.barplot(
        #     x=list(improvements_lut.keys()),
        #     y=list(improvements_lut.values()),
        #     ax=axs[1],
        #     log_scale=(False, True),
        # )
        # axs[1].set_yscale("log")
        # axs[1].set_title("LUT Improvement over Baseline")
        # axs[1].set_ylabel("Improvement Factor")
        # axs[1].set_xlabel("Design Name")

        axs[1].grid(axis="y", which="major", linestyle="-", linewidth=1.0, alpha=0.9)
        axs[1].grid(axis="y", which="minor", linestyle="--", linewidth=0.5, alpha=0.7)
        axs[1].set_axisbelow(True)
        sns.barplot(
            # x=list(improvements_alp.keys()),
            # y=list(improvements_alp.values()),
            x=[item[0] for item in data_alp_sorted],
            y=[item[1] for item in data_alp_sorted],
            ax=axs[1],
            log_scale=(False, True),
            # hue="#ce4257",
        )
        # set all the bar colors
        for i, patch in enumerate(axs[1].patches):
            patch.set_facecolor("#ce4257")

        axs[1].set_yscale("log")
        axs[1].set_title(
            "Area-Latency Product Improvement over Baseline", fontsize=TITLE_FONT_SIZE
        )
        axs[1].set_ylabel("Improvement Ratio", fontsize=LABEL_FONT_SIZE)
        # axs[1].set_xlabel("Design Name")
        axs[1].tick_params(axis="x", rotation=45)
        for label in axs[1].get_xticklabels():
            label.set_horizontalalignment("right")
        # labels = [f"{tick.get_text()}x" for tick in axs[1].get_yticklabels()]
        # axs[1].set_yticklabels(labels)
        axs[1].yaxis.set_major_formatter(FuncFormatter(lambda y, _: f"{int(y)}x"))
        # set y lim to end at 100x
        axs[1].set_ylim(top=100)

        # fig, ax = plt.subplots(figsize=(10, 6))
        # sns.barplot(
        #     x=list(improvements_alp.keys()),
        #     y=list(improvements_alp.values()),
        #     ax=ax,
        #     # log_scale=(False, True),
        # )
        # ax.set_yscale("log")
        # ax.set_title("Area-Latency Product Improvement over Baseline")
        # ax.set_ylabel("Improvement Factor")
        # ax.set_xlabel("Design Name")

        fig.tight_layout()
        fig.savefig(fname=DIR_FIGURES / "improvements_over_baseline.png", dpi=300)
