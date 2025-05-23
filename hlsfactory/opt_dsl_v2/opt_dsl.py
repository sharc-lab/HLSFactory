import random
import re
from collections import defaultdict


class OptDSL:
    def __init__(self, opt_dsl_source: str) -> None:
        self.pipeline_directives = defaultdict(list)
        self.unroll_directives = defaultdict(list)
        self.partition_directives = defaultdict(list)
        self.parse_opt_dsl_lang(opt_dsl_source)

    def parse_opt_dsl_lang(self, opt_dsl_source):
        opt_dsl_source = re.sub(r"pipeline\s*\(", "self.pipeline(", opt_dsl_source)
        opt_dsl_source = re.sub(r"unroll\s*\(", "self.unroll(", opt_dsl_source)
        opt_dsl_source = re.sub(r"partition\s*\(", "self.partition(", opt_dsl_source)
        #############################################################################
        # exec is potentially unsafe, but seems like the easiest way to go about this
        #############################################################################
        exec(opt_dsl_source)

    def pipeline(self, label: str, function: str) -> None:
        directive = f"set_directive_pipeline {function}/{label}"
        self.pipeline_directives[f"{function}/{label}"].append(directive)

    def unroll(self, label: str, function: str, factor: int) -> None:
        directive = f"set_directive_unroll -factor {factor} {function}/{label}"
        self.unroll_directives[f"{function}/{label}"].append(directive)

    def partition(
        self, array_var: str, function: str, partition_type: str, factor: int, dim: int
    ) -> None:
        directive = f"set_directive_array_partition -type {partition_type} -factor {factor} -dim {dim} {function} {array_var}"
        self.partition_directives[f"{function}/{array_var}"].append(directive)

    def get_directives(self):
        return (
            self.pipeline_directives,
            self.unroll_directives,
            self.partition_directives,
        )

    # TODO
    def add_static_optimization(self, opt_name: str) -> None:
        raise NotImplementedError
