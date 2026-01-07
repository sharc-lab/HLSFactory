import csv
import enum
import io
import json
import multiprocessing
import uuid
from abc import ABC, abstractmethod
from dataclasses import dataclass, field
from pathlib import Path
from zipfile import ZIP_DEFLATED, ZipFile

import pandas as pd

from hlsfactory.flow_vitis import auto_find_solutions
from hlsfactory.framework import Design

InMemoryArchive = io.BytesIO


@dataclass
class CompleteHLSData:
    design: dict | None
    synthesis: dict | None
    implementation: dict | None
    execution: dict | None
    artifacts: InMemoryArchive | None
    design_id: str = field(
        default_factory=lambda: str(uuid.uuid4()),
    )

    def to_flat_dict(self) -> dict:
        data = {}

        data["design_id"] = self.design_id

        if self.design is not None:
            for key, value in self.design.items():
                data[f"design__{key}"] = value

        if self.synthesis is not None:
            for key, value in self.synthesis.items():
                data[f"synthesis__{key}"] = value

        if self.implementation is not None:
            for key, value in self.implementation.items():
                data[f"implementation__{key}"] = value

        if self.execution is not None:
            for key, value in self.execution.items():
                data[f"execution__{key}"] = value

        return data

    def to_json(self) -> str:
        return json.dumps([self.to_flat_dict()], indent=4)

    def to_json_file(self, file_path: Path) -> None:
        file_path.write_text(json.dumps([self.to_flat_dict()], indent=4))

    def to_csv(self) -> str:
        csv_data = [self.to_flat_dict()]
        csv_string = io.StringIO()
        writer = csv.DictWriter(csv_string, fieldnames=csv_data[0].keys())
        writer.writeheader()
        writer.writerows(csv_data)
        return csv_string.getvalue()

    def to_csv_file(self, file_path: Path) -> None:
        file_path.write_text(self.to_csv())


class DataAggregator(ABC):
    VENDER = None

    @abstractmethod
    def gather_hls_design_data(self, design: Design) -> dict: ...

    @abstractmethod
    def gather_hls_synthesis_data(self, design: Design) -> dict: ...

    @abstractmethod
    def gather_implementation_data(self, design: Design) -> dict: ...

    @abstractmethod
    def gather_execution_data(self, design: Design) -> dict: ...

    @abstractmethod
    def gather_hls_synthesis_artifacts(
        self,
        design: Design,
    ) -> InMemoryArchive: ...

    def gather_all_data(self, design: Design, **kwargs) -> CompleteHLSData:
        hls_design_data = self.gather_hls_design_data(design, **kwargs)
        hls_synthesis_data = self.gather_hls_synthesis_data(design, **kwargs)
        execution_data = self.gather_execution_data(design, **kwargs)
        implementation_data = self.gather_implementation_data(design, **kwargs)
        artifacts = self.gather_hls_synthesis_artifacts(design, **kwargs)

        return CompleteHLSData(
            design=hls_design_data,
            synthesis=hls_synthesis_data,
            implementation=implementation_data,
            execution=execution_data,
            artifacts=artifacts,
        )

    def gather_multiple_designs(
        self,
        designs: list[Design],
        n_jobs: int = 1,
    ) -> list[CompleteHLSData]:
        if n_jobs < 1:
            raise ValueError("n_jobs must be greater than 0")
        if n_jobs == 1:
            return [self.gather_all_data(design) for design in designs]
        pool = multiprocessing.Pool(n_jobs)
        return pool.map(self.gather_all_data, designs)

    def aggregated_data_to_csv(self, data: list[CompleteHLSData]) -> str:
        s = io.StringIO()
        df_agg = pd.DataFrame([d.to_flat_dict() for d in data])
        df_agg.to_csv(s, index=False)
        return s.getvalue()

    def aggregated_data_to_csv_file(
        self,
        data: list[CompleteHLSData],
        file_path: Path,
    ) -> None:
        data_str = self.aggregated_data_to_csv(data)
        file_path.write_text(data_str)

    def aggregated_data_to_json(self, data: list[CompleteHLSData]) -> str:
        s = io.StringIO()
        df_agg = pd.DataFrame([d.to_flat_dict() for d in data])
        df_agg.to_json(s, orient="records", indent=4)
        return s.getvalue()

    def aggregated_data_to_json_file(
        self,
        data: list[CompleteHLSData],
        file_path: Path,
    ) -> None:
        data_str = self.aggregated_data_to_json(data)
        file_path.write_text(data_str)

    def aggregated_data_to_archive(
        self,
        data: list[CompleteHLSData],
        file_path: Path,
    ) -> None:
        json_data_all = self.aggregated_data_to_json(data)
        csv_data_all = self.aggregated_data_to_csv(data)
        with ZipFile(file_path, "w", ZIP_DEFLATED) as archive:
            archive.writestr("data_all.json", json_data_all)
            archive.writestr("data_all.csv", csv_data_all)
            for _i, d in enumerate(data):
                design_id = d.design_id
                json_data = d.to_json()
                csv_data = d.to_csv()
                archive.writestr(f"{design_id}/data.json", json_data)
                archive.writestr(f"{design_id}/data.csv", csv_data)
                if d.artifacts is not None:
                    d.artifacts.seek(0)
                    archive.writestr(f"{design_id}/artifacts.zip", d.artifacts.read())


ArtifactCollection = dict[str, list[Path] | None]


class ArtifactsXilinx(enum.Enum):
    IR = "ir"
    ADB = "adb"
    REPORT = "report"
    HDL = "hdl"
    IP = "ip"


def get_file_in_root(dir_fp: Path, file_name: str) -> dict:
    data_fp = dir_fp / file_name
    if not data_fp.exists():
        # raise FileNotFoundError(f"Data file not found: {data_fp}")
        print(f"WARNING: Data file not found: {data_fp}")
        return {}
    print(f"DATA FOUND: {data_fp}")
    return json.loads(data_fp.read_text())


class DataAggregatorXilinx(DataAggregator):
    VENDER = "Xilinx"

    def gather_hls_design_data(self, design: Design) -> dict:
        return get_file_in_root(design.dir, "data_design.json")

    def gather_hls_synthesis_data(self, design: Design) -> dict:
        return get_file_in_root(design.dir, "data_hls.json")

    def gather_implementation_data(self, design: Design) -> dict:
        return get_file_in_root(design.dir, "data_implementation.json")

    def gather_execution_data(self, design: Design) -> dict:
        return get_file_in_root(design.dir, "execution_time_data.json")

    def gather_hls_synthesis_artifacts_data(
        self,
        design: Design,
        artifacts_to_extract: set[ArtifactsXilinx] = {  # noqa: B006
            ArtifactsXilinx.IR,
            ArtifactsXilinx.ADB,
            ArtifactsXilinx.REPORT,
            ArtifactsXilinx.HDL,
            ArtifactsXilinx.IP,
        },
        error_if_missing_data: bool = True,
    ) -> ArtifactCollection:
        if artifacts_to_extract is None:
            raise ValueError("You specified no artifacts to extract")

        data: ArtifactCollection = {}

        if (design.dir / "timeout__VitisHLSSynthFlow.txt").exists():
            print(
                "WARNING: Timeout file found, synthesis never completed, no artifacts to extract",
            )
            return {}

        if (design.dir / "error__VitisHLSSynthFlow.txt").exists():
            print(
                "WARNING: Error file found, synthesis never completed, no artifacts to extract",
            )
            return {}

        solutions = auto_find_solutions(design.dir)
        if len(solutions) != 1:
            if error_if_missing_data:
                raise ValueError(f"Found 0 or more than 1 solution for {design.dir}")
            return {}
        solution = solutions[0]

        adb_fp = Path(solution) / ".autopilot" / "db"
        if not adb_fp.exists():
            if error_if_missing_data:
                raise FileNotFoundError(f"Autopilot DB directory not found: {adb_fp}")
            return {}

        report_fp = Path(solution) / "syn" / "report"
        if not report_fp.exists():
            raise FileNotFoundError(f"Report directory not found: {report_fp}")

        if ArtifactsXilinx.IR in artifacts_to_extract:
            bitcode_fp = adb_fp / "a.o.3.bc"
            if not bitcode_fp.exists():
                raise FileNotFoundError(f"Bitcode file not found: {bitcode_fp}")
            data["ir"] = [bitcode_fp]
        else:
            data["ir"] = None

        if ArtifactsXilinx.ADB in artifacts_to_extract:
            # find all files that end in .adb or .adb.xml but
            # have no other periods in the name
            adb_files = list(adb_fp.glob("*.adb"))
            adb_files = list(filter(lambda x: x.name.count(".") == 1, adb_files))
            if len(adb_files) == 0:
                raise FileNotFoundError(f"No .adb files found in {adb_fp}")
            adb_xml_files = list(adb_fp.glob("*.adb.xml"))
            adb_xml_files = list(
                filter(lambda x: x.name.count(".") == 2, adb_xml_files),  # noqa: PLR2004
            )
            if len(adb_xml_files) == 0:
                raise FileNotFoundError(f"No .adb.xml files found in {adb_fp}")
            data["adb"] = adb_files + adb_xml_files
        else:
            data["adb"] = None

        if ArtifactsXilinx.REPORT in artifacts_to_extract:
            report_files = list(report_fp.glob("*.xml")) + list(report_fp.glob("*.rpt"))
            if len(report_files) == 0:
                raise FileNotFoundError(f"No report files found in {report_fp}")
            data["report"] = report_files
        else:
            data["report"] = None

        if ArtifactsXilinx.HDL in artifacts_to_extract:
            hdl_dir_verilog = Path(solution) / "syn" / "verilog"
            hdl_files_verilog = list(hdl_dir_verilog.rglob("*.v"))
            data["hdl"] = hdl_files_verilog
        else:
            data["hdl"] = None

        if ArtifactsXilinx.IP in artifacts_to_extract:
            ip_dir = Path(solution) / "impl" / "ip"
            search = list(ip_dir.glob("*.zip"))
            if len(search) == 0:
                raise FileNotFoundError(f"No IP zip files found in {ip_dir}")
            if len(search) > 1:
                raise ValueError(f"Found more than 1 IP zip file in {ip_dir}")
            ip_zip = search[0]
            if not ip_zip.exists():
                raise FileNotFoundError(f"IP zip file not found: {ip_zip}")
            data["ip"] = [ip_zip]

        return data

    def gather_hls_synthesis_artifacts(
        self,
        design: Design,
        artifacts_to_extract: set[ArtifactsXilinx] = {  # noqa: B006
            ArtifactsXilinx.IR,
            ArtifactsXilinx.ADB,
            ArtifactsXilinx.REPORT,
            ArtifactsXilinx.HDL,
            ArtifactsXilinx.IP,
        },
        error_if_missing_data: bool = True,
    ) -> InMemoryArchive:
        data = self.gather_hls_synthesis_artifacts_data(
            design,
            artifacts_to_extract=artifacts_to_extract,
            error_if_missing_data=error_if_missing_data,
        )

        archive_buffer = io.BytesIO()
        archive = ZipFile(archive_buffer, "w", ZIP_DEFLATED)
        top_level = "artifacts"
        for key, files in data.items():
            if files is not None:
                for file in files:
                    archive.write(file, f"{top_level}/{key}/{file.name}")
        archive.close()
        return archive_buffer
