import json
import re
from dataclasses import dataclass
from pathlib import Path

from hlsfactory.flow_intel import (
    DesignHLSSynthData,
    IntelHLSDesign,
    IntelImpDesignResource,
)
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    call_tool,
    find_bin_path,
    serialize_methods_for_dataclass,
)


@serialize_methods_for_dataclass
@dataclass
class AlteraHLSSynthData:
    # todo...
    # todo...

    @classmethod
    def parse_from_synth_report_file(cls, data_file: Path) -> "AlteraHLSSynthData":
        raise NotImplementedError("AlteraHLSSynthData is not implemented yet.")


@serialize_methods_for_dataclass
@dataclass
class AlteraHLSDesign:
    # todo...

    @classmethod
    def parse_from_synth_report_file(
        cls,
        info_json: Path,
        summary_json: Path,
    ) -> "AlteraHLSDesign":
        raise NotImplementedError("AlteraHLSDesign is not implemented yet.")


class AlteraHLSSynthFlow(ToolFlow):
    name = "AlteraHLSSynthFlow"

    def __init__(
        self,
        # todo...
    ) -> None:
        raise NotImplementedError("AlteraHLSSynthFlow is not implemented yet.")

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        raise NotImplementedError("AlteraHLSSynthFlow is not implemented yet.")


@serialize_methods_for_dataclass
@dataclass
class AlteraImplDesignResource:
    # todo...

    @classmethod
    def parse_from_impl_report_file(
        cls,
        quartus_json: Path,
    ) -> "AlteraImplDesignResource":
        raise NotImplementedError("AlteraImplDesignResource is not implemented yet.")


class AlteraQuartusImplFlow(ToolFlow):
    name = "AlteraQuartusImplFlow"

    def __init__(self) -> None:
        raise NotImplementedError("AlteraQuartusImplFlow is not implemented yet.")

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        raise NotImplementedError("AlteraQuartusImplFlow is not implemented yet.")
