import asyncio
import json
import os
import pickle
import sys
import time
from dataclasses import dataclass
from pathlib import Path

from lightningsim.model import Solution
from lightningsim.runner import Runner, RunnerStep
from lightningsim.trace_file import ResolvedTrace

from hlsfactory.design_config import FlowName
from hlsfactory.framework import Design, ToolFlow
from hlsfactory.utils import (
    flow_already_completed,
    log_execution_time_to_file,
    timeout_not_supported,
)


@dataclass
class EvalResult:
    fifo_sizes: dict[int, int]
    deadlock: bool

    latency: float | None
    bram_usage_total: int | None

    timestamp: float | None = None


class LSEnv:
    """Wraps a Vitis HLS solution's LightningSim trace and evaluates FIFO-sized designs."""

    def __init__(
        self,
        vitis_hls_solution_dir: Path,
        env_vars_extra: dict[str, str] | None = None,
    ) -> None:
        self.vitis_hls_solution_dir = vitis_hls_solution_dir
        self.env_vars_extra = env_vars_extra or {}

        for key, value in self.env_vars_extra.items():
            os.environ[key] = value

        trace_pickle_fp = vitis_hls_solution_dir / "trace.pkl"
        try:
            with open(trace_pickle_fp, "rb") as f:
                self.trace_base: ResolvedTrace = pickle.load(f)
                print("Loaded trace from pickle file.")

        except FileNotFoundError:
            solution = Solution(self.vitis_hls_solution_dir)
            runner = Runner(solution)

            runner.steps[RunnerStep.ANALYZING_PROJECT].on_start(
                lambda _: print("Analyzing project...")
            )
            runner.steps[RunnerStep.WAITING_FOR_BITCODE].on_start(
                lambda _: print("Waiting for bitcode to be generated...")
            )
            runner.steps[RunnerStep.GENERATING_SUPPORT_CODE].on_start(
                lambda _: print("Generating support code...")
            )
            runner.steps[RunnerStep.LINKING_BITCODE].on_start(
                lambda _: print("Linking bitcode...")
            )
            runner.steps[RunnerStep.COMPILING_BITCODE].on_start(
                lambda _: print("Compiling bitcode...")
            )
            runner.steps[RunnerStep.LINKING_TESTBENCH].on_start(
                lambda _: print("Linking testbench...")
            )
            runner.steps[RunnerStep.RUNNING_TESTBENCH].on_start(
                lambda _: print("Running testbench...")
            )
            runner.steps[RunnerStep.PARSING_SCHEDULE_DATA].on_start(
                lambda _: print("Parsing schedule data from C synthesis...")
            )
            runner.steps[RunnerStep.RESOLVING_TRACE].on_start(
                lambda _: print("Resolving dynamic schedule from trace...")
            )

            sys.setrecursionlimit(10_000)

            self.trace_base: ResolvedTrace = asyncio.run(runner.run())  # type: ignore
            with open(trace_pickle_fp, "wb") as f:
                pickle.dump(self.trace_base, f)
                print("Saved trace to pickle file.")

        self.fifos = self.trace_base.fifos
        self.num_fifos = len(self.trace_base.fifos)

        self.fifo_sizes_base: dict[int, int | None] = {}
        for fifo in self.fifos:
            fifo_id: int = fifo.id
            fifo_depth: int | None = self.trace_base.params.fifo_depths[fifo_id]
            self.fifo_sizes_base[fifo_id] = fifo_depth

    def eval_solution_single(self, x: dict[int, int]) -> EvalResult:
        base_params = self.trace_base.params

        dse_results = self.trace_base.compiled.dse(base_params, [x])
        t = time.perf_counter()
        dse_result = dse_results[0]

        fifo_sizes = x
        if dse_result.latency is None:
            deadlock = True
            latency = None
            bram_usage_total = None
        else:
            deadlock = False
            latency = dse_result.latency
            bram_usage_total = dse_result.bram_count

        return EvalResult(
            fifo_sizes=fifo_sizes,
            deadlock=deadlock,
            latency=latency,
            bram_usage_total=bram_usage_total,
            timestamp=t,
        )

    def eval_solution_default(self) -> EvalResult:
        fifo_sizes_base_not_none: dict[int, int] = {}
        for fifo_id, fifo_size in self.fifo_sizes_base.items():
            if fifo_size is None:
                raise ValueError(
                    f"FIFO size for FIFO {fifo_id} is None. Please set a valid FIFO size."
                )
            fifo_sizes_base_not_none[fifo_id] = fifo_size
        return self.eval_solution_single(fifo_sizes_base_not_none)


class LightningSimFlow(ToolFlow):
    name = "LightningSimFlow"

    def __init__(
        self,
        log_execution_time: bool = True,
    ) -> None:
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        if timeout is not None:
            timeout_not_supported(self.name)

        design_dir = design.dir

        if flow_already_completed(
            design_dir,
            self.name,
            success_marker_fp=design_dir / "data_lightningsim.json",
        ):
            print(f"[{design_dir}] Skipping {self.name}, already completed")
            return [design]

        config = design.require_config()
        solution_dir_name = config.require_flow_setting(
            FlowName.LIGHTNING_SIM, "solution_dir_name"
        )
        solution_dir = design_dir / solution_dir_name

        if not solution_dir.is_dir():
            raise FileNotFoundError(
                f"LightningSim solution directory {solution_dir} does not exist. "
                f"Ensure C synthesis (csynth) has been run for design '{design.name}'."
            )

        t_0 = time.perf_counter()

        prj_path = design_dir.resolve().absolute()
        sim_env = LSEnv(
            solution_dir,
            env_vars_extra={"PRJ_PATH": str(prj_path)},
        )

        baseline_results = sim_env.eval_solution_default()

        if baseline_results.deadlock:
            (design_dir / f"error__{self.name}.txt").touch()
            print(f"[{design_dir}] LightningSim baseline simulation deadlocked")

            t_1 = time.perf_counter()
            if self.log_execution_time:
                log_execution_time_to_file(design_dir, self.name, t_0, t_1)

            return []

        data = {
            "design_name": design.name,
            "deadlock": baseline_results.deadlock,
            "fifo_sizes": baseline_results.fifo_sizes,
            "latency": baseline_results.latency,
            "bram_usage_total": baseline_results.bram_usage_total,
        }
        (design_dir / "data_lightningsim.json").write_text(json.dumps(data, indent=4))

        t_1 = time.perf_counter()
        if self.log_execution_time:
            log_execution_time_to_file(design_dir, self.name, t_0, t_1)

        return [design]
