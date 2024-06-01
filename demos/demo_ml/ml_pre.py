import re
import xml.etree.ElementTree as ET
import zipfile
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

print(data_all["synthesis__clock_period"])


def get_report_table_block(rpt_raw: str, block_name: str) -> str | None:
    rpt_lines = rpt_raw.split("\n")
    line_idx_start = None
    line_idx_end = None
    for idx, line in enumerate(rpt_lines):
        if f"* {block_name}:" in line:
            line_idx = idx
            line_idx_start = idx + 1
        if line_idx_start is not None and line_idx is not None and line.strip() == "":
            line_idx_end = idx
            break
    assert line_idx_start is not None
    assert line_idx_end is not None

    block = "\n".join(line.strip() for line in rpt_lines[line_idx_start:line_idx_end])
    if block.strip() == "N/A":
        return None
    return block


def parse_ascii_line(line: str) -> list[str]:
    line = line.strip()
    parts = line.split("|")
    parts = [part.strip() for part in parts if part.strip() != ""]
    return parts


def parse_ascii_table(table: str) -> pd.DataFrame:
    table_lines = table.split("\n")
    header = parse_ascii_line(table_lines[1])
    data = [parse_ascii_line(line) for line in table_lines[3:-3]]

    return pd.DataFrame(data, columns=header)


ops_to_check_for = [
    "mul",
    "add",
    "sub",
    "or",
    "and",
    "xor",
    "shl",
    "lshr",
    "ashr",
    "ctlz",
    "cttz",
    "icmp",
    "select",
]

bind_to_check_for = [
    "dexp",
    "ext",
    "fsub",
    "srem",
    "frem",
    "dmul",
    "urem",
    "fmul",
    "frecip",
    "dsqrt",
    "drem",
    "others",
    "icmp",
    "fsqrt",
    "dsub",
    "ddiv",
    "sub",
    "lshr",
    "sdiv",
    "dadd",
    "vectors",
    "frsqrt",
    "mux",
    "fcmp",
    "drecip",
    "add",
    "fdiv",
    "dcmp",
    "drsqrt",
    "fadd",
    "mul",
    "udiv",
    "shl",
    "ashr",
]

extra_data = []

for design_id in data_all["design_id"]:
    print(design_id)

    data = {}
    data["design_id"] = design_id

    with zipfile.ZipFile(zip_fp, "r") as z:
        artifacts_zip_fp = f"{design_id}/artifacts.zip"
        buf_artifacts = BytesIO(z.read(artifacts_zip_fp))

    report_files = []
    with zipfile.ZipFile(buf_artifacts, "r") as z:
        for name in z.namelist():
            # keep file name not path
            name_base = name.split("/")[-1]
            if re.match(r".*\.rpt", name_base):
                report_files.append(name_base)

    report_files = sorted(report_files)
    report_files.remove("csynth.rpt")
    report_files.remove("csynth_design_size.rpt")

    expression_data = {}
    mux_data = {}

    df_expressions_all = []
    df_mux_all = []
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

            # replace + with add, - with sub, * with mul, / with div
            table_df["Operation"] = table_df["Operation"].str.replace("+", "add")
            table_df["Operation"] = table_df["Operation"].str.replace("-", "sub")
            table_df["Operation"] = table_df["Operation"].str.replace("*", "mul")
            table_df["Operation"] = table_df["Operation"].str.replace("/", "div")

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

    if df_mux_all is not []:
        df_mux = pd.concat(df_mux_all)
        mux_data["mux_lut_sum"] = df_mux["LUT"].sum()
        mux_data["mux_input_size_sum"] = df_mux["Input Size"].sum()
        mux_data["mux_bits_sum"] = df_mux["Bits"].sum()
        mux_data["mux_total_bits_sum"] = df_mux["Total Bits"].sum()

    data_bind = {}

    # open csynth.xml
    with zipfile.ZipFile(buf_artifacts, "r") as z:
        with z.open("artifacts/report/csynth.xml") as f:
            csynth_xml_raw = f.read().decode("utf-8")

    # parse csynth.xml
    csynth_xml = ET.fromstring(csynth_xml_raw)

    # find all BindNode elements
    bind_nodes = csynth_xml.findall(".//BindNode")
    # get the BINDTYPE attribute of each BindNode
    # bind_types = [bind_node.attrib["BINDTYPE"] for bind_node in bind_nodes]
    # pp(bind_types)
    # keep only if BINDTYPE is "op"
    bind_nodes = [
        bind_node for bind_node in bind_nodes if bind_node.attrib["BINDTYPE"] == "op"
    ]

    optypes = [bind_node.attrib["OPTYPE"] for bind_node in bind_nodes]
    bind_dsp = [bind_node.attrib["DSP"] for bind_node in bind_nodes]
    bind_latency = [bind_node.attrib["LATENCY"] for bind_node in bind_nodes]

    # for op in optypes:
    #     assert op in bind_to_check_for, f"op {op} not in bind_to_check_for"

    # for bind_op in bind_to_check_for:
    #     data_bind[f"bind_{bind_op}_count"] = optypes.count(bind_op)

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

extra_data_df = pd.DataFrame(extra_data)

expression_cols = [col for col in extra_data_df.columns if "expression__" in col]
mux_cols = [col for col in extra_data_df.columns if "mux__" in col]

data_all = data_all.merge(extra_data_df, on="design_id")
data_all.to_csv(DATA_DIR / "data_all.csv", index=False)
