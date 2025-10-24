import json
from pathlib import Path
from zipfile import ZipFile

import matplotlib.pyplot as plt
import pandas as pd
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler

DIR_CURRENT = Path(__file__).parent
FP_DATA = DIR_CURRENT / "extracted_trace_run32hours.zip"

DIR_FIGURES = DIR_CURRENT / "figures"
DIR_FIGURES.mkdir(exist_ok=True)


def load_data(fp_data: Path) -> dict[str, pd.DataFrame]:
    df_map = {}
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
    return df_map


if __name__ == "__main__":
    df_map = load_data(FP_DATA)
    for design_name, df in df_map.items():
        print(f"Design: {design_name}, DataFrame shape: {df.shape}")

    for design_name, df in df_map.items():
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
        pca = PCA(n_components=2)
        principal_components = pca.fit_transform(x_scaled)
        df_pca = pd.DataFrame(
            data=principal_components,
            columns=["principal_component_1", "principal_component_2"],
        )
        df_pca["valid"] = y.to_numpy()
        plt.figure()
        plt.scatter(
            df_pca["principal_component_1"],
            df_pca["principal_component_2"],
            c=df_pca["valid"],
            alpha=0.5,
        )
        plt.xlabel("Principal Component 1")
        plt.ylabel("Principal Component 2")
        plt.title(f"PCA of {design_name}")
        plt.colorbar(label="Valid")
        plt.savefig(DIR_FIGURES / f"pca__{design_name}.png", dpi=300)
        plt.close()

        # Pareto plot for valid designs
        df_valid = df[df["valid"]]
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

        plt.figure()
        plt.plot(x_pareto, y_front, "r-", linewidth=2, label="Pareto front")
        plt.scatter(
            df_valid["used_LUT"], df_valid["perf"], alpha=0.5, label="Valid designs"
        )

        # Mark the design with the lowest area-latency product
        best_x = best_alp_row["used_LUT"]
        best_y = best_alp_row["perf"]
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
        plt.annotate(
            f"ALP: {best_alp_row['area_latency_product']:.2f}\nLUT: {int(best_x)}\nLat: {best_y}",
            xy=(best_x, best_y),
            xytext=(10, -30),
            textcoords="offset points",
            fontsize=8,
            bbox=dict(boxstyle="round,pad=0.3", fc="yellow", alpha=0.6),
            arrowprops=dict(arrowstyle="->", connectionstyle="arc3,rad=0"),
        )
        # make log log
        plt.xscale("log")
        plt.yscale("log")

        plt.xlabel("LUTs")
        plt.ylabel("Latency")
        plt.title(f"Pareto Plot for {design_name}")
        plt.legend()
        plt.savefig(DIR_FIGURES / f"pareto__{design_name}.png", dpi=300)
        plt.close()
