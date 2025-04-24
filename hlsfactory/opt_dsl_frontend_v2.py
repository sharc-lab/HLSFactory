import hashlib
import itertools
import json
import random
import time
from pathlib import Path

from hlsfactory.framework import Design, Frontend
from hlsfactory.opt_dsl_v2.opt_dsl import OptDSL
from hlsfactory.utils import log_execution_time_to_file


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
    pipelines,
    unrolls,
    partitions,
    random_sample: bool = False,
    random_sample_num: int = 10,
    random_sample_seed: int = 42,
) -> list[str]:
    all_directives = pipelines | unrolls | partitions

    if random_sample and random_sample_num < count_possible_samples(all_directives):
        # Ensure that we're not sampling more than the size of the powerset
        design_space = unique_random_samples(all_directives, random_sample_num)
    else:
        # None represents no directive chosen for that loop/variable
        design_space = list(
            itertools.product(*([[None] + lst for lst in all_directives.items()]))
        )

    opt_tcl_sources: list[str] = []
    # opt_configs = []
    for design_point in design_space:
        opt_tcl_source = ""

        for directive in design_point:
            if directive:
                opt_tcl_source += directive + "\n"

        opt_tcl_sources.append(opt_tcl_source)

        # config_single = {}

        # config_single["pipelines"] = []
        # for x in pipeline:
        #     opt_tcl_source += x + "\n"
        #     config_single["pipelines"].append(x)

        # config_single["unrolls"] = []
        # for x in unroll:
        #     opt_tcl_source += x + "\n"
        #     config_single["unrolls"].append(x)

        # config_single["paritions"] = []
        # for x in partition:
        #     opt_tcl_source += x + "\n"
        #     config_single["partitions"].append(x)

        # opt_configs.append(config_single)

    return opt_tcl_sources
    # return opt_tcl_sources, opt_configs


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

        with open(opt_template_fp) as file:
            opt_dsl = OptDSL(file.read())

        pipelines, unrolls, partitions = opt_dsl.get_directives()

        opt_sources = generate_opt_sources(
            pipelines,
            unrolls,
            partitions,
            self.random_sample,
            self.random_sample_num,
            self.random_sample_seed,
        )

        new_designs = []
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
