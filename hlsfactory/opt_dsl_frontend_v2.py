import hashlib
import itertools
import json
import random
import time
import shutil
import signal
from contextlib import contextmanager
from collections import defaultdict
from pathlib import Path

from hlsfactory.framework import Design, Frontend
from hlsfactory.opt_dsl_v2.opt_dsl import OptDSL
from hlsfactory.utils import log_execution_time_to_file

@contextmanager
def timeout_guard(seconds: float | None, *, label: str = "operation"):
    if seconds is None:
        yield
        return
    if not hasattr(signal, "SIGALRM"):
        raise NotImplementedError("timeout_guard requires SIGALRM (POSIX/Linux).")

    def _handler(signum, frame):
        raise TimeoutError(f"{label} timed out after {seconds}s")

    old = signal.getsignal(signal.SIGALRM)
    signal.signal(signal.SIGALRM, _handler)
    signal.setitimer(signal.ITIMER_REAL, float(seconds))
    try:
        yield
    finally:
        signal.setitimer(signal.ITIMER_REAL, 0.0)
        signal.signal(signal.SIGALRM, old)


def count_possible_samples(data):
    total = 1
    for values in data.values():
        total *= 1 + len(values)
    return total


def unique_random_samples(data, num_samples):
    seen_samples = set()

    values_list = list(data.values())

    while len(seen_samples) < num_samples:
        selected_lists = random.sample(values_list, random.randint(1, len(values_list)))
        sample = frozenset(random.choice(lst) for lst in selected_lists)
        seen_samples.add(sample)

    return list(seen_samples)


def generate_opt_sources(
    static_lines: list[str],
    groups,
    pipelines,
    partitions,
    unrolls,
    random_sample: bool = False,
    random_sample_num: int = 10,
    random_sample_seed: int = 42,
) -> list[str]:
    group_names = list(groups.keys())
    group_factor_options = [list(groups[group].keys()) for group in group_names]
    group_factor_all = list(itertools.product(*group_factor_options))

    grouped_options = []
    for factors in group_factor_all:
        grouped_directives = []
        for group, factor in zip(group_names, factors, strict=False):
            grouped_directives.extend(
                groups[group][factor]
            )  # list of directives from that factor
        grouped_options.append(grouped_directives)

    ungrouped_directives = []
    ungrouped_options = []
    for directive_dict in [pipelines, unrolls, partitions]:
        for dir_list in directive_dict.values():
            ungrouped_directives.append(dir_list)
    ungrouped_options = list(itertools.product(*ungrouped_directives))

    design_space = []
    for grouped_option in grouped_options:
        for ungrouped_option in ungrouped_options:
            directives = grouped_option + [d for d in ungrouped_option if d is not None]
            design_space.append(directives)

    if random_sample:
        random.seed(random_sample_seed)
        if random_sample_num < len(design_space):
            samples = random.sample(design_space, random_sample_num)
        else:
            samples = design_space

    else:
        samples = design_space

    opt_tcl_sources = []

    for sample in samples:
        opt_tcl_source = ""

        # Static lines always included
        for line in static_lines:
            opt_tcl_source += line + "\n"

        # Dynamic directives
        for entry in sample:
            if isinstance(entry, list):
                for directive in entry:
                    opt_tcl_source += directive + "\n"
            elif entry is not None:
                opt_tcl_source += entry + "\n"

        opt_tcl_sources.append(opt_tcl_source)

    return opt_tcl_sources


class OptDSLFrontend(Frontend):
    name = "OptDSLFrontend"

    def __init__(
        self,
        work_dir: Path,
        random_sample: bool = False,
        random_sample_num: int = 10,
        random_sample_seed: int = 42,
        log_execution_time: bool = True,
    ) -> None:
        self.work_dir = work_dir
        self.random_sample = random_sample
        self.random_sample_num = random_sample_num
        self.random_sample_seed = random_sample_seed
        self.log_execution_time = log_execution_time

    def execute(self, design: Design, timeout: float | None = None) -> list[Design]:
        t_0 = time.perf_counter()

        opt_template_fp = design.dir / "opt_template.tcl"
        opt_dsl = None
        new_designs = []

        try:
            with timeout_guard(timeout, label=f"OptDSL frontend execute (design={design.name})"):
                with open(opt_template_fp) as file:
                    opt_dsl = OptDSL(file.read())
                
                if opt_dsl.opt_dsl_error:
                    raise ValueError(opt_dsl.error_message)

                static_lines, groups, pipelines, partitions, unrolls = opt_dsl.get_directives()

                opt_sources = generate_opt_sources(
                    static_lines,
                    groups,
                    pipelines,
                    partitions,
                    unrolls,
                    self.random_sample,
                    self.random_sample_num,
                    self.random_sample_seed,
                )

                # TODO Add config tracking back in
                for opt_source in opt_sources:
                    # TODO Come up with better naming scheme?
                    opt_source_hash = hashlib.md5(opt_source.encode()).hexdigest()
                    new_design = design.copy_and_rename_to_new_parent_dir(
                        f"{design.name}_opt_{opt_source_hash}",
                        design.dir.parent,
                    )
                    opt_fp = new_design.dir / "opt.tcl"
                    opt_fp.write_text(opt_source)

                    # opt_config_fp = new_design.dir / "opt_config.json"
                    # opt_config_fp.write_text(json.dumps(opt_config, indent=4))

                    new_designs.append(new_design)

                    t_1 = time.perf_counter()
                    if self.log_execution_time:
                        log_execution_time_to_file(new_design.dir, self.name, t_0, t_1)

                t_1 = time.perf_counter()
                if self.log_execution_time:
                    log_execution_time_to_file(design.dir, self.name, t_0, t_1)
                    
                return new_designs
        
        except TimeoutError as e:
            print(f"TimeoutError in OptDSLFrontend for design {design.name}: {e}")
            if self.log_execution_time:
                t_1 = time.perf_counter()
                log_execution_time_to_file(design.dir, f"{self.name}__TIMEOUT", t_0, t_1)
            return new_designs

        except shutil.Error as e:
            if self.log_execution_time:
                t_1 = time.perf_counter()
                log_execution_time_to_file(design.dir, f"{self.name}__TIMEOUT", t_0, t_1)
            return new_designs
