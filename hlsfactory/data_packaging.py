import csv
import enum
import io
import json
import multiprocessing
import uuid
from abc import ABC, abstractmethod
from collections.abc import Iterable
from dataclasses import dataclass, field
from pathlib import Path
from typing import ClassVar
from zipfile import ZIP_DEFLATED, ZipFile

import pandas as pd

from hlsfactory.flow_vitis import auto_find_solutions
from hlsfactory.framework import Design
from hlsfactory.utils import (
    ExecutionData,
    ExecutionDataStatus,
    FlowExecutionData,
    read_execution_data,
)

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


class JSONFileDataAggregator(DataAggregator):
    """Shared base for aggregators.

    Every flow writes its results as JSON files in the design directory
    (``data_design.json``, ``data_hls.json``, ``data_implementation.json`` and
    ``execution_data.json``), so reading them does not depend on the vendor.
    Subclasses add the vendor-specific part: collecting the artifacts a flow
    leaves on disk.
    """

    def gather_hls_design_data(self, design: Design, **_kwargs) -> dict:
        return get_file_in_root(design.dir, "data_design.json")

    def gather_hls_synthesis_data(self, design: Design, **_kwargs) -> dict:
        return get_file_in_root(design.dir, "data_hls.json")

    def gather_implementation_data(self, design: Design, **_kwargs) -> dict:
        return get_file_in_root(design.dir, "data_implementation.json")

    def gather_execution_data(self, design: Design, **_kwargs) -> dict:
        fp = design.dir / "execution_data.json"
        if not fp.exists():
            return get_file_in_root(design.dir, "execution_data.json")
        execution_data = read_execution_data(design.dir)
        if not isinstance(execution_data, ExecutionData):
            return {}
        return execution_data.to_dict()


class DataAggregatorXilinx(JSONFileDataAggregator):
    VENDER = "Xilinx"

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
        error_if_missing_data: bool = False,
    ) -> ArtifactCollection:
        if artifacts_to_extract is None:
            raise ValueError("You specified no artifacts to extract")

        data: ArtifactCollection = {}

        # Check execution_data.json for error or timeout
        exec_data_fp = design.dir / "execution_data.json"
        if exec_data_fp.exists():
            try:
                synth_data = read_execution_data(
                    design.dir,
                    flow_name="VitisHLSSynthFlow",
                )
                if isinstance(synth_data, FlowExecutionData) and synth_data.status in (
                    ExecutionDataStatus.ERROR,
                    ExecutionDataStatus.TIMEOUT,
                ):
                    print(
                        f"WARNING: VitisHLSSynthFlow marked as {synth_data.status.value}, synthesis never completed, no artifacts to extract",
                    )
                    return {}
            except Exception:
                pass

        try:
            solutions = auto_find_solutions(design.dir)
        except FileNotFoundError:
            if error_if_missing_data:
                raise
            return {}
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
            if error_if_missing_data:
                raise FileNotFoundError(f"Report directory not found: {report_fp}")
            return {}

        def found(files: list[Path], description: str) -> list[Path]:
            if not files and error_if_missing_data:
                raise FileNotFoundError(f"No {description} found")
            return files

        if ArtifactsXilinx.IR in artifacts_to_extract:
            bitcode_fp = adb_fp / "a.o.3.bc"
            data["ir"] = found(
                [bitcode_fp] if bitcode_fp.exists() else [],
                f"bitcode file {bitcode_fp}",
            )
        else:
            data["ir"] = None

        if ArtifactsXilinx.ADB in artifacts_to_extract:
            # find all files that end in .adb or .adb.xml but
            # have no other periods in the name
            adb_files = list(adb_fp.glob("*.adb"))
            adb_files = list(filter(lambda x: x.name.count(".") == 1, adb_files))
            adb_xml_files = list(adb_fp.glob("*.adb.xml"))
            adb_xml_files = list(
                filter(lambda x: x.name.count(".") == 2, adb_xml_files),  # noqa: PLR2004
            )
            found(adb_files, f".adb files in {adb_fp}")
            found(adb_xml_files, f".adb.xml files in {adb_fp}")
            data["adb"] = adb_files + adb_xml_files
        else:
            data["adb"] = None

        if ArtifactsXilinx.REPORT in artifacts_to_extract:
            report_files = list(report_fp.glob("*.xml")) + list(report_fp.glob("*.rpt"))
            data["report"] = found(report_files, f"report files in {report_fp}")
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
            if len(search) > 1 and error_if_missing_data:
                raise ValueError(f"Found more than 1 IP zip file in {ip_dir}")
            data["ip"] = found(search[:1], f"IP zip files in {ip_dir}")
        else:
            data["ip"] = None

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
        error_if_missing_data: bool = False,
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


class ArtifactAggregator(JSONFileDataAggregator):
    """Aggregator that collects vendor artifacts by file-name pattern.

    Subclasses declare:

    * ``ARTIFACTS``: an enum of artifact kinds (what can be collected).
    * ``ARTIFACT_PATTERNS``: for each kind, glob patterns relative to the
      design directory. Only files are collected.
    * ``SYNTH_FLOW_NAME``: the flow whose recorded status in
      ``execution_data.json`` decides whether synthesis completed.

    Artifacts are returned as ``{kind.value: [paths]}`` and archived as
    ``artifacts/<kind>/<path relative to the design directory>``.
    """

    ARTIFACTS: ClassVar[type[enum.Enum]]
    ARTIFACT_PATTERNS: ClassVar[dict[enum.Enum, tuple[str, ...]]]
    SYNTH_FLOW_NAME: ClassVar[str]

    def _synthesis_incomplete(self, design: Design) -> bool:
        if not (design.dir / "execution_data.json").exists():
            return False
        try:
            flow_data = read_execution_data(
                design.dir,
                flow_name=self.SYNTH_FLOW_NAME,
            )
        except Exception:
            return False
        if isinstance(flow_data, FlowExecutionData) and flow_data.status in (
            ExecutionDataStatus.ERROR,
            ExecutionDataStatus.TIMEOUT,
        ):
            print(
                f"WARNING: {self.SYNTH_FLOW_NAME} marked as {flow_data.status.value}, "
                "synthesis never completed, no artifacts to extract",
            )
            return True
        return False

    def gather_hls_synthesis_artifacts_data(
        self,
        design: Design,
        artifacts_to_extract: Iterable[enum.Enum] | None = None,
        error_if_missing_data: bool = False,
    ) -> ArtifactCollection:
        """Find the requested artifact kinds on disk.

        Args:
            design: Design whose directory is searched.
            artifacts_to_extract: Kinds to collect; defaults to every kind.
            error_if_missing_data: If True, raise ``FileNotFoundError`` when a
                requested kind matches no files. By default (False) that kind
                maps to ``[]`` and collection continues.
        """
        kinds = (
            list(self.ARTIFACTS)
            if artifacts_to_extract is None
            else list(artifacts_to_extract)
        )
        for kind in kinds:
            if not isinstance(kind, self.ARTIFACTS):
                raise ValueError(
                    f"{kind!r} is not a {self.ARTIFACTS.__name__} artifact kind",
                )

        if self._synthesis_incomplete(design):
            return {}

        data: ArtifactCollection = {kind.value: None for kind in self.ARTIFACTS}
        for kind in kinds:
            found: set[Path] = set()
            for pattern in self.ARTIFACT_PATTERNS[kind]:
                found.update(p for p in design.dir.glob(pattern) if p.is_file())
            if not found and error_if_missing_data:
                raise FileNotFoundError(
                    f"No {kind.value} artifacts found in {design.dir} "
                    f"(patterns: {', '.join(self.ARTIFACT_PATTERNS[kind])})",
                )
            data[kind.value] = sorted(found)
        return data

    def gather_hls_synthesis_artifacts(
        self,
        design: Design,
        artifacts_to_extract: Iterable[enum.Enum] | None = None,
        error_if_missing_data: bool = False,
        **_kwargs,
    ) -> InMemoryArchive:
        data = self.gather_hls_synthesis_artifacts_data(
            design,
            artifacts_to_extract=artifacts_to_extract,
            error_if_missing_data=error_if_missing_data,
        )
        archive_buffer = io.BytesIO()
        with ZipFile(archive_buffer, "w", ZIP_DEFLATED) as archive:
            for kind, files in data.items():
                for file in files or []:
                    rel = file.relative_to(design.dir)
                    archive.write(file, f"artifacts/{kind}/{rel.as_posix()}")
        return archive_buffer


class ArtifactsXLS(enum.Enum):
    IR = "ir"
    RTL = "rtl"
    METRICS = "metrics"
    INTERFACE = "interface"
    OPTIONS = "options"
    SOURCE_MAP = "source_map"
    DEBUG = "debug"


class DataAggregatorXLS(ArtifactAggregator):
    """Google XLS: IR at each stage, RTL, signature/schedule/metrics, options."""

    VENDER = "XLS"
    ARTIFACTS = ArtifactsXLS
    SYNTH_FLOW_NAME = "XLSHLSSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsXLS.IR: ("*.ir",),
        ArtifactsXLS.RTL: ("*.v",),
        ArtifactsXLS.METRICS: (
            "*.signature.textproto",
            "*.block_metrics.textproto",
            "*.schedule.textproto",
            "*pass_metrics.textproto",
        ),
        ArtifactsXLS.INTERFACE: ("*.interface.pb", "*.interface.textproto"),
        ArtifactsXLS.OPTIONS: ("*_options.textproto",),
        ArtifactsXLS.SOURCE_MAP: ("*.verilog_line_map.textproto",),
        ArtifactsXLS.DEBUG: (
            "*.optimizer_ir_dumps/**/*",
            "*.codegen_ir_dumps/**/*",
            "*.pprof",
        ),
    }


class ArtifactsCatapult(enum.Enum):
    REPORT = "report"
    RTL = "rtl"


class DataAggregatorCatapult(ArtifactAggregator):
    """Siemens Catapult: ``*.rpt`` reports and generated RTL."""

    VENDER = "Catapult"
    ARTIFACTS = ArtifactsCatapult
    SYNTH_FLOW_NAME = "CatapultHLSSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsCatapult.REPORT: ("**/*.rpt",),
        ArtifactsCatapult.RTL: (
            "**/rtl.v",
            "**/concat_rtl.v",
            "**/rtl.vhdl",
            "**/concat_rtl.vhdl",
        ),
    }


class ArtifactsStratus(enum.Enum):
    RTL = "rtl"
    REPORT = "report"
    LOG = "log"


class DataAggregatorStratus(ArtifactAggregator):
    """Cadence Stratus: generated RTL, the BDR report, and metrics/log files."""

    VENDER = "Stratus"
    ARTIFACTS = ArtifactsStratus
    SYNTH_FLOW_NAME = "StratusHLSSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsStratus.RTL: ("**/*_rtl.v",),
        ArtifactsStratus.REPORT: (
            "**/stratus_hls.bdr",
            ".hlsfactory_stratus_metrics.tsv",
        ),
        ArtifactsStratus.LOG: ("**/stratus_hls.log",),
    }


class ArtifactsIntel(enum.Enum):
    REPORT = "report"
    HDL = "hdl"
    QUARTUS = "quartus"


class DataAggregatorIntel(ArtifactAggregator):
    """Intel HLS Compiler (``i++``) and Quartus: reports, HDL, Quartus reports."""

    VENDER = "Intel"
    ARTIFACTS = ArtifactsIntel
    SYNTH_FLOW_NAME = "IntelHLSSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsIntel.REPORT: (
            "*.prj/reports/report.html",
            "*.prj/reports/lib/json/*.json",
        ),
        ArtifactsIntel.HDL: (
            "*.prj/components/**/*.v",
            "*.prj/components/**/*.sv",
            "*.prj/components/**/*.vhd",
        ),
        ArtifactsIntel.QUARTUS: (
            "*.prj/quartus/*.qsf",
            "*.prj/quartus/**/*.rpt",
            "*.prj/quartus/**/*.summary",
        ),
    }


class ArtifactsAltera(enum.Enum):
    REPORT = "report"
    INTERFACE = "interface"
    HDL = "hdl"
    LOG = "log"


class DataAggregatorAltera(ArtifactAggregator):
    """Altera HLS IP Gen and Quartus: reports, interface spec, RTL, logs."""

    VENDER = "Altera"
    ARTIFACTS = ArtifactsAltera
    SYNTH_FLOW_NAME = "AlteraHLSSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsAltera.REPORT: (
            "**/*.prj/reports/report.html",
            "**/*.prj/reports/resources/json/*.ndjson",
        ),
        ArtifactsAltera.INTERFACE: ("**/*.prj/ipinterfaces.xml",),
        ArtifactsAltera.HDL: ("**/*.prj/**/*.sv", "**/*.prj/**/*.v"),
        ArtifactsAltera.LOG: ("**/flow.log",),
    }


class ArtifactsVitisModern(enum.Enum):
    REPORT = "report"
    HDL = "hdl"


class DataAggregatorVitisModern(ArtifactAggregator):
    """Config-file Vitis HLS (``v++``): ``csynth`` reports and Verilog/VHDL."""

    VENDER = "Xilinx"
    ARTIFACTS = ArtifactsVitisModern
    SYNTH_FLOW_NAME = "VitisHLSModernSynthFlow"
    ARTIFACT_PATTERNS = {
        ArtifactsVitisModern.REPORT: (
            "**/hls/syn/report/*.xml",
            "**/hls/syn/report/*.rpt",
        ),
        ArtifactsVitisModern.HDL: (
            "**/hls/syn/verilog/*.v",
            "**/hls/syn/vhdl/*.vhd",
        ),
    }
