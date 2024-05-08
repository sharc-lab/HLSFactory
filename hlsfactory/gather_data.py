import argparse
import json
from enum import Enum
from pathlib import Path

import pandas as pd


def main(args: argparse.Namespace) -> None:
    file_format = args.format
    search_dir = args.search_dir
    output_file = args.output_file

    data_design_fps = list(search_dir.rglob("**/data_design.json"))
    data_designs = [json.loads(fp.read_text()) for fp in data_design_fps]

    design_dirs = [fp.parent for fp in data_design_fps]
    data_hls_fps = [design_dir / "data_hls.json" for design_dir in design_dirs]
    data_hls = [json.loads(fp.read_text()) for fp in data_hls_fps]

    data = []
    for data_design, data_hls_single in zip(data_designs, data_hls, strict=False):
        data.append({**data_design, **data_hls_single})

    # code to handle differnt data files with different sets of keys
    data_dfs = []
    for idx, d in enumerate(data):
        df_single = pd.DataFrame(d, index=[idx])
        data_dfs.append(df_single)
    df_combined = pd.concat(data_dfs)

    if file_format == FileFormat.CSV:
        df_combined.to_csv(output_file, index=False)

    if file_format == FileFormat.JSON:
        df_combined.to_json(output_file, orient="records", indent=4)

    if file_format == FileFormat.SQLITE:
        raise NotImplementedError("sqlite output not implemented yet")


class FileFormat(Enum):
    CSV = "csv"
    JSON = "json"
    SQLITE = "sqlite"


if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-f",
        "--format",
        type=FileFormat,
        choices=list(FileFormat),
        default=FileFormat.JSON,
        help="Output file format",
    )

    parser.add_argument(
        "search_dir",
        type=Path,
        help="Directory to search for generated HLS data",
    )
    parser.add_argument(
        "output_file",
        type=Path,
        help="Output file to write the aggregated data to",
    )
    args = parser.parse_args()

    main(args)
